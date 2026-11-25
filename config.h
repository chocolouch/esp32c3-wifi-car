// =============================================================================
//  config.h  -  all user-adjustable settings for the ESP32-C3 Robot Car
//  Target: Arduino-ESP32 core 3.x (uses the 3.x LEDC API: ledcAttach/ledcWrite)
// =============================================================================
#pragma once

#include <Arduino.h>
#include <WiFi.h>

#if !defined(ESP_ARDUINO_VERSION_MAJOR) || (ESP_ARDUINO_VERSION_MAJOR < 3)
#error "This project needs the 'esp32 by Espressif Systems' board package 3.x (Boards Manager). The 2.x LEDC API is NOT supported."
#endif

// -----------------------------------------------------------------------------
//  GPIO assignments (your mapping, unchanged)
// -----------------------------------------------------------------------------
constexpr uint8_t PIN_LEFT_IN1  = 3;   // L298N IN1  - left motor direction
constexpr uint8_t PIN_LEFT_IN2  = 4;   // L298N IN2  - left motor direction
constexpr uint8_t PIN_RIGHT_IN3 = 5;   // L298N IN3  - right motor direction
constexpr uint8_t PIN_RIGHT_IN4 = 6;   // L298N IN4  - right motor direction
constexpr uint8_t PIN_LEFT_ENA  = 7;   // L298N ENA  - left motor PWM speed
constexpr uint8_t PIN_RIGHT_ENB = 10;  // L298N ENB  - right motor PWM speed

// -----------------------------------------------------------------------------
//  Wi-Fi access point
// -----------------------------------------------------------------------------
constexpr char    WIFI_AP_SSID[]     = "RobotCar-C3";
constexpr char    WIFI_AP_PASSWORD[] = "robotcar123";   // 8..63 characters. CHANGE THIS.
static_assert(sizeof(WIFI_AP_PASSWORD) - 1 >= 8 && sizeof(WIFI_AP_PASSWORD) - 1 <= 63,
              "WIFI_AP_PASSWORD must be 8 to 63 characters");

constexpr uint8_t WIFI_AP_CHANNEL     = 1;              // 1..11 (2.4 GHz)
constexpr uint8_t WIFI_AP_MAX_CLIENTS = 2;
constexpr uint8_t WIFI_AP_IP[4]       = {192, 168, 4, 1};  // page: http://192.168.4.1

// Many ESP32-C3 SuperMini boards have a weak antenna / supply and behave better
// at reduced TX power. Raise to WIFI_POWER_15dBm if range is a problem.
constexpr wifi_power_t WIFI_TX_POWER = WIFI_POWER_8_5dBm;

// -----------------------------------------------------------------------------
//  Motor direction (software reversal, no rewiring)
//  Forward = IN1 HIGH / IN2 LOW (left), IN3 HIGH / IN4 LOW (right).
//  If a wheel turns the wrong way when "forward" is commanded, set it to true.
// -----------------------------------------------------------------------------
constexpr bool LEFT_MOTOR_REVERSED  = false;
constexpr bool RIGHT_MOTOR_REVERSED = false;

// -----------------------------------------------------------------------------
//  PWM (ESP32-C3 LEDC, core 3.x API)
//   frequency  : 2000 Hz
//   resolution : 8 bit -> duty range 0..255
//   Stop = duty 0 and both IN pins LOW (motor coasts).
// -----------------------------------------------------------------------------
constexpr uint32_t PWM_FREQUENCY_HZ    = 2000;
constexpr uint8_t  PWM_RESOLUTION_BITS = 8;
constexpr uint16_t PWM_DUTY_LIMIT      = (1u << PWM_RESOLUTION_BITS) - 1;   // 255

// UI speed 1..100 % is mapped linearly onto PWM_MIN_DUTY..PWM_MAX_DUTY.
//  PWM_MIN_DUTY: lowest duty at which the TT motors reliably start turning.
// PWM_MAX_DUTY: Software speed cap.
// This is NOT a voltage regulator and does NOT guarantee safe motor voltage.
// The 2S battery must not be connected directly to the motor supply
// until a suitable motor-voltage arrangement has been established.
constexpr uint16_t PWM_MIN_DUTY = 90;
constexpr uint16_t PWM_MAX_DUTY = 200;
static_assert(PWM_MIN_DUTY >= 1 && PWM_MIN_DUTY <= PWM_MAX_DUTY && PWM_MAX_DUTY <= PWM_DUTY_LIMIT,
              "Need 1 <= PWM_MIN_DUTY <= PWM_MAX_DUTY <= 255");

// -----------------------------------------------------------------------------
//  Safety
// -----------------------------------------------------------------------------
// If no valid /drive command arrives within this time, both motors stop.
// The web page sends a command every 100 ms while a control is held.
constexpr uint32_t COMMAND_TIMEOUT_MS = 500;
static_assert(COMMAND_TIMEOUT_MS >= 250, "Timeout shorter than ~250 ms will cause stuttering");

// -----------------------------------------------------------------------------
//  Debug
// -----------------------------------------------------------------------------
constexpr bool     DEBUG_SERIAL = true;
constexpr uint32_t SERIAL_BAUD  = 115200;
