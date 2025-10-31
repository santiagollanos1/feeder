#include <Arduino.h>
#include "dephased_pwm.h"
#include "settings.h"


DephasedPWM::DephasedPWM() {}

void DephasedPWM::init() {


//Paso 1: Habilitar el reloj GCLK1 y conectarlo a los timers TCC0 y TCC1 (necesito dos timers, uno para cada pwm)


  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |  // divide la frec. de reloj en 1 (queda en 48MHz)
                    GCLK_GENDIV_ID(4); // aplico a generic clock 4
  while (GCLK->STATUS.bit.SYNCBUSY);


  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | // IDC = "Improve Duty Cycle"  set 50/50?
                     GCLK_GENCTRL_GENEN | // Enable generic glock gen
                     GCLK_GENCTRL_SRC_DFLL48M |// Select 48MHz as source
                     GCLK_GENCTRL_ID(4); // Select generic clock 4 
  while (GCLK->STATUS.bit.SYNCBUSY);  // wait for synchronization



// Paso 2: Conectar los pines a los TCC. Para esto se debe configurar el multiplexor
//El F indica que se conecta al timer TCC0
  PORT->Group[g_APinDescription[7].ulPort].PINCFG[g_APinDescription[7].ulPin].bit.PMUXEN  = 1;
  PORT->Group[g_APinDescription[6].ulPort].PMUX[g_APinDescription[6].ulPin >> 1].reg = PORT_PMUX_PMUXO_F;

  PORT->Group[g_APinDescription[3].ulPort].PINCFG[g_APinDescription[3].ulPin].bit.PMUXEN  = 1;
  PORT->Group[g_APinDescription[2].ulPort].PMUX[g_APinDescription[2].ulPin >> 1].reg = PORT_PMUX_PMUXO_F;



  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);   

// Paso 3: Aplico el prescaler a los timers y establecer el modo PWM

  REG_TCC0_WAVE |= TCC_WAVE_POL(0xF) |         // Reverse the output polarity on all TCC0 outputs
                    TCC_WAVE_WAVEGEN_DSBOTH;    // Setup dual slope PWM on TCC0
  while (TCC0->SYNCBUSY.bit.WAVE);               // Wait for synchronization

  REG_TCC1_WAVE |= TCC_WAVE_POL(0x3) |         // Reverse the output polarity on all TCC0 outputs
                    TCC_WAVE_WAVEGEN_DSBOTH;    // Setup dual slope PWM on TCC0
  while (TCC1->SYNCBUSY.bit.WAVE);               // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: 
  REG_TCC0_PER = 600;         // Set the frequency of the PWM on TCC0 to 40kHz
  while (TCC0->SYNCBUSY.bit.PER);                // Wait for synchronization


  // Set the PWM signal to output 50% duty cycle
  REG_TCC0_CC3 = 300;         // TCC0 CC3 - on D7
  while (TCC0->SYNCBUSY.bit.CC3);     

  REG_TCC1_PER = 100;         // Set the frequency of the PWM on TCC0 to 40kHz
  while (TCC1->SYNCBUSY.bit.PER);                // Wait for synchronization
  
  REG_TCC1_CCB1 = 500;         // TCC0 CC3 - on D7
  while (TCC1->SYNCBUSY.bit.CCB1);     


// Paso 4: Configurar los pines de salida

  // Divide the 48MHz signal by 1 giving 48MHz (20.83ns) TCC0 timer tick and enable the outputs
  REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV1 |    // Divide GCLK4 by 1
                    TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization

  REG_TCC1_CTRLA |= TCC_CTRLA_PRESCALER_DIV1 |    // Divide GCLK4 by 1
                    TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC1->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
  
}


//Paso 6: enjoy!



void DephasedPWM::set_phase(uint32_t phase_shift){
  TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
  while (TCC0->SYNCBUSY.bit.COUNT);
  TCC1->COUNT.reg = (TCC0->COUNT.reg + phase_shift) % period;

}



