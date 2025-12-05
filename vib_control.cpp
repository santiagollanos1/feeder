#include "vib_control.h"

VibrationControl::VibrationControl(int pinNumber) {
    pin = pinNumber;
    vibration_time = 35;
    interval = 2000;
    
    vibration_enabled = false;
    previousMillis = 0;
    relayState = false;
    currentDuration = interval;
}

void VibrationControl::init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void VibrationControl::run() {
    vibration_enabled = true;
    previousMillis = millis(); 
    relayState = false;
    currentDuration = 0;
}

void VibrationControl::stop() { 
    vibration_enabled = false;
    relayState = false;
    digitalWrite(pin, LOW);
}

void VibrationControl::vibrator() {
    if (vibration_enabled) {
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= currentDuration) {
            previousMillis = currentMillis; 
            relayState = !relayState;

            if (relayState) { 
                currentDuration = vibration_time;
            } else {
                currentDuration = interval;
            }
            
            digitalWrite(pin, relayState ? HIGH : LOW); 
        }
    }
}

void VibrationControl::set_seedint(uint32_t seedint) {
    interval = seedint;
}

void VibrationControl::set_vibtime(uint32_t vibtime) {
    vibration_time = vibtime;
}
