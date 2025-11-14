#include "dephased_pwm.h"
#include "serial_command.h"
#include "settings.h"


volatile uint32_t phase_angle = 0;   // fase global
DephasedPWM pwm;                     // tu objeto

void TC5_Handler(void) {
    // limpiar interrupción
    TC5->COUNT16.INTFLAG.bit.MC0 = 1;

    // llamar a la función de fase
    pwm.set_phase(phase_angle);

    phase_angle = (phase_angle + 45) % 360;  // desfasar cada interrupción
    Serial.print(phase_angle);
}


void setup() {
  Serial.begin(115200);
  
  pwm.init_phaseloop(11500);
  //pwm.set_phase(360);
  pwm.init();
}




void loop() {
}
  

