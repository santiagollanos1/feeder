#include <Arduino.h>
#include "inodriver_bridge.h"



void setup() {
    Serial.begin(115200);
    bridge_setup();
}

void loop() {
    bridge_loop();
}
  
