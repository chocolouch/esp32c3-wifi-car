// motor_control.cpp
#include "motor_control.h"
#include "config.h"

namespace {

struct Motor {
  uint8_t  in1;
  uint8_t  in2;
  uint8_t  en;        // PWM pin
  bool     reversed;
  int      percent;   // last applied, -100..100
  uint16_t duty;      // last applied PWM duty
};

Motor gLeft  = {PIN_LEFT_IN1,  PIN_LEFT_IN2,  PIN_LEFT_ENA,  LEFT_MOTOR_REVERSED,  0, 0};
Motor gRight = {PIN_RIGHT_IN3, PIN_RIGHT_IN4, PIN_RIGHT_ENB, RIGHT_MOTOR_REVERSED, 0, 0};

bool gPwmReady = false;

uint16_t percentToDuty(int magnitude) {            // magnitude 1..100
  if (magnitude <= 0) return 0;
  if (magnitude > 100) magnitude = 100;
  return (uint16_t)(PWM_MIN_DUTY +
         ((uint32_t)(magnitude - 1) * (PWM_MAX_DUTY - PWM_MIN_DUTY)) / 99);
}

void stopOne(Motor& m) {
  if (gPwmReady) ledcWrite(m.en, 0);
  digitalWrite(m.in1, LOW);
  digitalWrite(m.in2, LOW);
  m.percent = 0;
  m.duty = 0;
}

void driveOne(Motor& m, int percent) {
  percent = constrain(percent, -100, 100);

  if (percent == 0 || !gPwmReady) {
    stopOne(m);
    return;
  }

  const bool forward = (percent > 0) != m.reversed;

  const uint8_t activePin =
      forward ? m.in1 : m.in2;

  const uint16_t duty =
      percentToDuty(abs(percent));

  // Disable motor output before changing direction.
  ledcWrite(m.en, 0);

  // Put both direction inputs LOW.
  digitalWrite(m.in1, LOW);
  digitalWrite(m.in2, LOW);

  // Brief transition interval.
  delayMicroseconds(5);

  // Set the new direction.
  digitalWrite(activePin, HIGH);

  // Apply the requested speed.
  ledcWrite(m.en, duty);

  m.percent = percent;
  m.duty = duty;
}

}  // namespace

bool motorsBegin() {
  Motor* all[] = {&gLeft, &gRight};

  // 1) Drive every pin LOW first so nothing moves.
  for (Motor* m : all) {
    pinMode(m->en, OUTPUT);  digitalWrite(m->en, LOW);
    pinMode(m->in1, OUTPUT); digitalWrite(m->in1, LOW);
    pinMode(m->in2, OUTPUT); digitalWrite(m->in2, LOW);
  }

  // 2) Attach PWM (core 3.x: pin-based API) and force duty 0.
  bool ok = true;
  for (Motor* m : all) {
    if (ledcAttach(m->en, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS)) {
      ledcWrite(m->en, 0);
    } else {
      ok = false;
    }
  }
  gPwmReady = ok;
  motorsStop();
  return ok;
}

void motorsSetPercent(int leftPercent, int rightPercent) {
  driveOne(gLeft, leftPercent);
  driveOne(gRight, rightPercent);
}

void motorsStop() {
  stopOne(gLeft);
  stopOne(gRight);
}

bool motorsActive()            { return gLeft.percent != 0 || gRight.percent != 0; }
int  motorsLeftPercent()       { return gLeft.percent; }
int  motorsRightPercent()      { return gRight.percent; }
uint16_t motorsLeftDuty()      { return gLeft.duty; }
uint16_t motorsRightDuty()     { return gRight.duty; }
