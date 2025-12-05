#include <Arduino.h>
#include "dephased_pwm.h"
#include "settings.h"

DephasedPWM::DephasedPWM() {}

void DephasedPWM::init() {

  // Paso 1: Habilitar reloj del bus para TCC0 , TCC1 y TC5
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1 | PM_APBCMASK_TC5;

  // Paso 2: Configurar reloj GCLK4 (48 MHz DFLL)
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(4);
  while (GCLK->STATUS.bit.SYNCBUSY);

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |
                     GCLK_GENCTRL_GENEN |
                     GCLK_GENCTRL_SRC_DFLL48M |
                     GCLK_GENCTRL_ID(4);
  while (GCLK->STATUS.bit.SYNCBUSY);

  // Paso 3: Conectar GCLK4 a TCC0 y TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |
                     GCLK_CLKCTRL_GEN_GCLK4 |
                     GCLK_CLKCTRL_ID_TCC0_TCC1;
  while (GCLK->STATUS.bit.SYNCBUSY);

  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |
                     GCLK_CLKCTRL_GEN_GCLK4 |
                     GCLK_CLKCTRL_ID_TC4_TC5;
  while (GCLK->STATUS.bit.SYNCBUSY);


  // --- TC5 
  {
    TC5->COUNT16.CTRLA.bit.SWRST = 1;
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);
    while (TC5->COUNT16.CTRLA.bit.SWRST);

    REG_TC5_CTRLA = TC_CTRLA_MODE_COUNT16 |
                    TC_CTRLA_PRESCALER_DIV1024 |
                    TC_CTRLA_WAVEGEN_MFRQ;

    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);
 
    TC5->COUNT16.CC[0].reg = 2343;   // 50MS POR DEFECTO
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    TC5->COUNT16.INTENSET.bit.MC0 = 1;
    NVIC_EnableIRQ(TC5_IRQn);

    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);
  }
  // --------------------------------------------------------


  // Paso 4: Configurar prescalers
  REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV1;
  REG_TCC1_CTRLA |= TCC_CTRLA_PRESCALER_DIV1;

  // Paso 5: Configurar modo PWM normal
  REG_TCC0_WAVE = TCC_WAVE_WAVEGEN_NPWM;
  while (TCC0->SYNCBUSY.bit.WAVE);

  REG_TCC1_WAVE = TCC_WAVE_WAVEGEN_NPWM;
  while (TCC1->SYNCBUSY.bit.WAVE);

  // Paso 6: Configurar periodo (frecuencia)
  uint32_t period = 1200;
  REG_TCC0_PER = period;
  while (TCC0->SYNCBUSY.bit.PER);
  REG_TCC1_PER = period;
  while (TCC1->SYNCBUSY.bit.PER);

  // Paso 7: Ciclo de trabajo 50%
  REG_TCC0_CC3 = period / 2;
  while (TCC0->SYNCBUSY.bit.CC3);
  REG_TCC1_CC1 = period / 2;
  while (TCC1->SYNCBUSY.bit.CC1);

  // Paso 8: Configurar multiplexor de pines
  PORT->Group[0].PINCFG[19].bit.PMUXEN = 1;
  PORT->Group[0].PMUX[19 >> 1].reg = PORT_PMUX_PMUXO_F;

  PORT->Group[0].PINCFG[9].bit.PMUXEN = 1;
  PORT->Group[0].PMUX[9 >> 1].reg = PORT_PMUX_PMUXO_F;

  // Paso 10: Habilitar los TCC
  REG_TCC0_CTRLA |= TCC_CTRLA_ENABLE;
  while (TCC0->SYNCBUSY.bit.ENABLE);
  REG_TCC1_CTRLA |= TCC_CTRLA_ENABLE;
  while (TCC1->SYNCBUSY.bit.ENABLE);
}

void DephasedPWM::set_phase(uint32_t phase_shift)
{
    TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
    while (TCC0->SYNCBUSY.bit.COUNT);

    uint32_t period = TCC0->PER.reg;
    uint32_t offset = (period * phase_shift) / 360;

    TCC1->COUNT.reg = (TCC0->COUNT.reg + offset) % period;
}


void DephasedPWM::set_frequency(uint32_t frequency) {
  REG_TCC0_PER = 48000000/frequency;
  while (TCC0->SYNCBUSY.bit.PER);
  REG_TCC1_PER = 48000000/frequency;
  while (TCC1->SYNCBUSY.bit.PER);
}

void DephasedPWM::phase_loop(uint32_t delta) { 
    TC5->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0; 
    
    this->set_phase(current_phase_angle); 
    
    current_phase_angle = (current_phase_angle + current_delta) % 360; // 
}

void DephasedPWM::set_delta(uint32_t delta) {
    current_delta = delta;
}

void DephasedPWM::set_timestep(uint32_t timestep) {
  
    uint32_t toggle_value = ((uint32_t)timestep * 46875UL) / (1000UL);
    if (toggle_value > 0 && toggle_value <= 65535) {
      TC5->COUNT16.CC[0].reg = (uint16_t)toggle_value;
      while (TC5->COUNT16.STATUS.bit.SYNCBUSY);
    }
}
