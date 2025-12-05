#ifndef DEPHASED_PWM_H
#define DEPHASED_PWM_H

#include <Arduino.h>

class DephasedPWM {
  public:
    DephasedPWM();                    
    void init();                      
    void set_phase(uint32_t shift);    
    void set_frequency(uint32_t frequency);
    void phase_loop(uint32_t delta);
    void set_timestep(uint32_t timestep);
    void set_delta(uint32_t delta);
  private:
    uint32_t current_phase_angle = 0; // Estado del ángulo
    uint32_t current_delta = 30;

};

#endif
