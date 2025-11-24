#include "serial_command.h"

extern volatile int phase_shift;
extern volatile uint32_t vibration_time; 
extern volatile long interval; 
extern volatile bool vibration_enabled;

void SerialCommand(DephasedPWM &pwm) {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim(); 

    if (cmd.startsWith("SET_PHASE")) {
      int val = cmd.substring(10).toInt();
      pwm.set_phase(val);
      Serial.print("Fase manual set a: "); Serial.println(val);
    }

    else if (cmd.startsWith("DELTA")) {
      int val = cmd.substring(6).toInt();
      phase_shift = val; 
      Serial.print("PHASE SHIFT "); Serial.println(phase_shift);
    }

    else if (cmd.startsWith("VIBTIME")) {
      int val = cmd.substring(8).toInt();
      vibration_time = val;
      Serial.print("VIBRATION TIME "); Serial.println(vibration_time);
    }
    
    else if (cmd.startsWith("SEEDINT")) {
      int val = cmd.substring(8).toInt();
      
      if (val >= 0) {
          interval = val; 
          Serial.print("SEED INTERVAL "); Serial.println(interval);
      } else {
          Serial.println("INTERVAL MUST BE POSITIVE");
      }
    }
    
    else if (cmd.startsWith("RUN")) {
      vibration_enabled = !vibration_enabled;
      Serial.print("VIBRATION "); 
      Serial.println(vibration_enabled ? "ON" : "OFF");
    }

    else if (cmd.startsWith("TIMESTEP")) {
      int time = cmd.substring(9).toInt();
      uint32_t toggle_value;

      toggle_value = ((uint32_t)time * 46875UL) / (1000UL);

      if (toggle_value > 0 && toggle_value <= 65535) {
        TC5->COUNT16.CC[0].reg = (uint16_t)toggle_value;
        while (TC5->COUNT16.STATUS.bit.SYNCBUSY); 
        
        Serial.print("TIME STEP "); Serial.println(time);
      } else {
        Serial.println("Error: periodo fuera de rangode 16bit");
      }
    }
  }
}
