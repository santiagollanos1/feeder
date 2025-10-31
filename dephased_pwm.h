#ifndef DEPHASED_PWM_H
#define DEPHASED_PWM_H

#include <Arduino.h>

class DephasedPWM {
  public:
    DephasedPWM();                    
    void init();                      
    void set_phase(uint32_t shift);    

  private:
    uint32_t period = 3500;          // periodo (top del timer)

};

#endif
