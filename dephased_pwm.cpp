#include <Arduino.h>
#include "dephased_pwm.h"
#include "settings.h"

DephasedPWM::DephasedPWM() {}

void DephasedPWM::init() {

  // Paso 1: Habilitar reloj del bus para TCC0 y TCC1
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1;

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
  REG_TCC0_CC3 = period / 2; // TCC0/WO[2] → D10 (PA18)
  while (TCC0->SYNCBUSY.bit.CC3);
  REG_TCC1_CC1 = period / 2; // TCC1/WO[1] → D9 (PA07)
  while (TCC1->SYNCBUSY.bit.CC1);

  // Paso 8: Configurar multiplexor de pines
  // PA19 → D10 → TCC0/WO[2] (función F)
  PORT->Group[0].PINCFG[19].bit.PMUXEN = 1;
  PORT->Group[0].PMUX[19 >> 1].reg = PORT_PMUX_PMUXO_F;

  // PA09 → D12 → TCC1/WO[1] (función E)
  PORT->Group[0].PINCFG[9].bit.PMUXEN = 1;
  PORT->Group[0].PMUX[9 >> 1].reg = PORT_PMUX_PMUXO_F;


  // Paso 10: Habilitar los TCC
  REG_TCC0_CTRLA |= TCC_CTRLA_ENABLE;
  while (TCC0->SYNCBUSY.bit.ENABLE);
  REG_TCC1_CTRLA |= TCC_CTRLA_ENABLE;
  while (TCC1->SYNCBUSY.bit.ENABLE);
}




void DephasedPWM::init_phaseloop(uint32_t timestep){
  PM->APBCMASK.reg |= PM_APBCMASK_TC5;

    // Conectar GCLK0 (48 MHz) al TC5
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK1 |
                        GCLK_CLKCTRL_ID_TC4_TC5;
  while (GCLK->STATUS.bit.SYNCBUSY);

  TC5->COUNT16.CTRLA.bit.SWRST = 1;
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);
  while (TC5->COUNT16.CTRLA.bit.SWRST);

  //TC5->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16;  // Modo 16 bits
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256;  // prescaler 1024

    // Cálculo para interrupciones exactas
  uint32_t compare = (48000000UL / 1024UL) * timestep / 1000UL;


  TC5->COUNT16.CC[1].reg = compare;
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    // Interrupción en canal 0
  TC5->COUNT16.INTENSET.bit.MC0 = 1;

    // habilitar NVIC
  NVIC_EnableIRQ(TC5_IRQn);

    // arrancar el timer
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);
}



void DephasedPWM::set_phase(uint32_t phase_shift) {
  // Leer y sincronizar el contador de TCC0


  TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
  while (TCC0->SYNCBUSY.bit.COUNT);

  // Aplicar el desfase en TCC1
  uint32_t period = TCC0->PER.reg;
  TCC1->COUNT.reg = (TCC0->COUNT.reg + period * phase_shift / 360 ) % period;

  //Serial.println(TCC1->COUNT.reg);
}



void DephasedPWM::set_phase(uint32_t phase_shift) {
  // Leer y sincronizar el contador de TCC0
  TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
  while (TCC0->SYNCBUSY.bit.COUNT);

  // Aplicar el desfase en TCC1
  uint32_t period = TCC0->PER.reg;
  TCC1->COUNT.reg = (TCC0->COUNT.reg + phase_shift) % period;

  Serial.println(TCC1->COUNT.reg);
}




