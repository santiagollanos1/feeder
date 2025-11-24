#ifndef VIB_CONTROL_H
#define VIB_CONTROL_H

#include <Arduino.h>

extern volatile uint32_t vibration_time;
extern volatile long interval;
extern volatile bool vibration_enabled;

void VibrationControl();

#endif