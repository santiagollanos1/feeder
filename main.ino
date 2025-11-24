#include "dephased_pwm.h"
#include "serial_command.h"
#include "settings.h"
#include "vib_control.h"

volatile uint32_t phase_angle = 0;
volatile int phase_shift = 30;
DephasedPWM pwm;

void TC5_Handler() 
{
    TC5->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0; 
    pwm.set_phase(phase_angle);
    phase_angle = (phase_angle + phase_shift) % 360;
}


void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    pinMode(1, OUTPUT);
    pwm.init();
}

void loop() {
    SerialCommand(pwm);
    VibrationControl(); 
}
  

void loop() {
}
  


