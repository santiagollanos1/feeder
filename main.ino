#include "dephased_pwm.h"
#include "serial_command.h"
#include "settings.h"

DephasedPWM pwm;

void setup() {
  Serial.begin(115200);
  pwm.init();
  
}

void loop() {
  static uint32_t phase = 0;
  pwm.set_phase(phase);
  phase = phase + 10;
  delay(10);
}
  
