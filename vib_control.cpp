#include "vib_control.h"

volatile uint32_t vibration_time = 35;
volatile long interval = 2000;
volatile bool vibration_enabled = true;
static unsigned long previousMillis = 0;
static bool relayState = false; 
static long currentDuration = interval;

void VibrationControl() {
    if (!vibration_enabled) {
            
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= currentDuration) {
            
            previousMillis = currentMillis; 
            relayState = !relayState;

            if (relayState == true) {
                currentDuration = vibration_time;
            } else {
                currentDuration = interval;
            }
            
            digitalWrite(1, relayState ? HIGH : LOW); 
        }
    }
}