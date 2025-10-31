#include "dephased_pwm.h"
#include "serial_command.h"
#include "settings.h"

DephasedPWM pwm;

void setup() {
  Serial.begin(115200);
  pwm.init();
  
}

void loop() {
  SerialCommand(pwm);  
}
