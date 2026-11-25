// motor_control.h - L298N dual-motor driver (differential drive)
#pragma once

#include <Arduino.h>

// Configure pins and PWM. Both motors are left stopped. Returns false if PWM
// setup failed (motors then stay disabled/stopped).
bool motorsBegin();

// Set both motors. Each value is -100..100 (negative = reverse, 0 = stop).
// Values are clamped. 1..100 maps onto PWM_MIN_DUTY..PWM_MAX_DUTY.
void motorsSetPercent(int leftPercent, int rightPercent);

// Immediately stop both motors (duty 0, IN pins LOW).
void motorsStop();

bool motorsActive();

int      motorsLeftPercent();
int      motorsRightPercent();
uint16_t motorsLeftDuty();
uint16_t motorsRightDuty();
