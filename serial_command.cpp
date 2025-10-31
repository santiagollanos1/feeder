// serial_command.cpp
#include "serial_command.h"

void SerialCommand(DephasedPWM &pwm) {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    if (cmd.startsWith("SET_PHASE")) {
      int val = cmd.substring(10).toInt();
      pwm.set_phase(val);
    }
  }
}
