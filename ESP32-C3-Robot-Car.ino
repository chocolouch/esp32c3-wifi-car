// =============================================================================
//  ESP32-C3 Wi-Fi Robot Car
//  Board package : esp32 by Espressif Systems, core 3.x
//  Board         : "ESP32C3 Dev Module"  (USB CDC On Boot: Enabled)
//  Controller    : join the Wi-Fi network, open http://192.168.4.1
// =============================================================================
#include <WiFi.h>
#include <WebServer.h>
#include <atomic>

#include "config.h"
#include "motor_control.h"
#include "web_interface.h"


WebServer server(80);

std::atomic<bool> gStopRequested{false};   // set from the Wi-Fi event task
uint32_t gLastCommandMs = 0;
int gLastLogLeft = 0;
int gLastLogRight = 0;

// -----------------------------------------------------------------------------
//  Wi-Fi access point
// -----------------------------------------------------------------------------
void onWiFiEvent(arduino_event_id_t event, arduino_event_info_t info) {
  if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
    gStopRequested.store(true, std::memory_order_relaxed);
  }
}

void startAccessPoint() {
  WiFi.onEvent(onWiFiEvent);
  WiFi.mode(WIFI_AP);

  const IPAddress ip(WIFI_AP_IP[0], WIFI_AP_IP[1], WIFI_AP_IP[2], WIFI_AP_IP[3]);
  const IPAddress mask(255, 255, 255, 0);
  WiFi.softAPConfig(ip, ip, mask);

  const bool ok = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL, false, WIFI_AP_MAX_CLIENTS);
  WiFi.setTxPower(WIFI_TX_POWER);

  if (DEBUG_SERIAL) {
    if (ok) {
      Serial.printf("Wi-Fi AP '%s' started. Open http://%s\n", WIFI_AP_SSID, WiFi.softAPIP().toString().c_str());
    } else {
      Serial.println("ERROR: Wi-Fi AP failed to start");
    }
  }
}

// -----------------------------------------------------------------------------
//  Web server: command handling
// -----------------------------------------------------------------------------
void sendStatusJson() {
  char buf[96];
  snprintf(buf, sizeof(buf), "{\"l\":%d,\"r\":%d,\"ld\":%u,\"rd\":%u}",
           motorsLeftPercent(), motorsRightPercent(),
           (unsigned)motorsLeftDuty(), (unsigned)motorsRightDuty());
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", buf);
}

// Accepts only a plain integer in -100..100.
bool parsePercentArg(const char* name, int& out) {
  if (!server.hasArg(name)) return false;
  String s = server.arg(name);
  if (s.length() == 0 || s.length() > 4) return false;
  char* end = nullptr;
  long v = strtol(s.c_str(), &end, 10);
  if (end == s.c_str() || *end != '\0' || v < -100 || v > 100) return false;
  out = (int)v;
  return true;
}

void logIfChanged() {
  if (!DEBUG_SERIAL) return;
  const int l = motorsLeftPercent(), r = motorsRightPercent();
  if (l != gLastLogLeft || r != gLastLogRight) {
    Serial.printf("L=%+d%% (duty %u)  R=%+d%% (duty %u)\n",
                  l, (unsigned)motorsLeftDuty(), r, (unsigned)motorsRightDuty());
    gLastLogLeft = l;
    gLastLogRight = r;
  }
}

void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache");
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleDrive() {
  int left = 0, right = 0;
  if (!parsePercentArg("l", left) || !parsePercentArg("r", right)) {
    // Malformed command: ignore it (motors unchanged; the timeout still applies).
    server.send(400, "application/json", "{\"error\":\"bad command\"}");
    return;
  }
  motorsSetPercent(left, right);
  gLastCommandMs = millis();
  logIfChanged();
  sendStatusJson();
}

void handleStop() {
  motorsStop();
  gLastCommandMs = millis();
  logIfChanged();
  sendStatusJson();
}

void handleStatus() {
  sendStatusJson();
}

void handleNoContent() {
  server.send(204);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void startWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/drive", HTTP_GET, handleDrive);
  server.on("/stop", HTTP_GET, handleStop);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/favicon.ico", HTTP_GET, handleNoContent);
  server.onNotFound(handleNotFound);
  server.begin();
}

// -----------------------------------------------------------------------------
//  Arduino entry points
// -----------------------------------------------------------------------------
void setup() {
  // Motors first, so every driver pin is held LOW as early as possible.
  const bool pwmOk = motorsBegin();

  Serial.begin(SERIAL_BAUD);
  const uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 1500) delay(10);   // don't hang if no monitor is open

  if (DEBUG_SERIAL) {
    Serial.println();
    Serial.println("ESP32-C3 Robot Car - motors stopped at boot");
    if (!pwmOk) Serial.println("ERROR: PWM init failed - motors disabled");
  }

  startAccessPoint();
  startWebServer();
  gLastCommandMs = millis();
}

void loop() {
  server.handleClient();

  if (gStopRequested.exchange(false, std::memory_order_relaxed)) {
    if (motorsActive()) {
      motorsStop();

      if (DEBUG_SERIAL) {
        Serial.println("Client disconnected -> motors stopped");
      }
    }
  }

  // Communication timeout (dead-man).
  if (motorsActive() &&
      (uint32_t)(millis() - gLastCommandMs) > COMMAND_TIMEOUT_MS) {
    motorsStop();

    if (DEBUG_SERIAL) {
      Serial.println("Command timeout -> motors stopped");
    }

    logIfChanged();
  }

  delay(1);
}