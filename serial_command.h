#ifndef SERIAL_COMMAND_H
#define SERIAL_COMMAND_H

#include <Arduino.h>
#include "dephased_pwm.h"
#include "vib_control.h"

extern volatile int phase_shift;

void SerialCommand(DephasedPWM &pwm);

#endif
