#ifndef VIB_CONTROL_H
#define VIB_CONTROL_H

#include <Arduino.h>

class VibrationControl {
  private:
    int pin;
    uint32_t vibration_time;
    unsigned long interval;

    bool vibration_enabled;
    unsigned long previousMillis;
    bool relayState;
    unsigned long currentDuration;

  public:
    VibrationControl(int pinNumber);
    void init();
    void run();
    void stop();
    void vibrator();
    void set_vibtime(uint32_t vibtime);
    void set_seedint(uint32_t seedint);
};

#endif