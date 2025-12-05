#include "inodriver_bridge.h"
#include "serial_command.h"

SerialCommand cmd;

void ok() {
  cmd.println("OK");
}

void error(const char* msg) {
  cmd.print("ERROR: ");
  cmd.println(msg);
}



void bridge_loop() {
  cmd.loop();   // buscar bien el nombre
  user_loop();
}

void bridge_setup() {
  cmd.setup();
  
  //// Setup callbacks for SerialCommand commands

  // All commands might return
  //    ERROR: <error message>

  // All set commands return 
  //    OK 
  // if the operation is successfull

  // All parameters are ascii encoded strings
  cmd.addCommand("INFO?", getInfo); 

  cmd.setDefaultHandler(unrecognized); 


  // Setter:
  //   FREQUENCY <I>
  // Returns: OK or ERROR 
  cmd.addCommand("FREQUENCY", wrapperSet_FREQUENCY); 


  /////////////////
  //// FIXED
  /////////////////

  // Setter:
  //   PHASE <I>
  // Returns: OK or ERROR 
  cmd.addCommand("PHASE", wrapperSet_PHASE); 


  // Setter:
  //   DELTA <I> 
  // Returns: OK or ERROR 
  cmd.addCommand("DELTA", wrapperSet_DELTA); 

  // Setter:
  //   TIMESTEP <I> 
  // Returns: OK or ERROR  
  cmd.addCommand("TIMESTEP", wrapperSet_TIMESTEP); 

  // Call:
  //   PAUSE
  // Returns: OK or ERROR  
//  cmd.addCommand("PAUSE", wrapperCall_PAUSE);   

  cmd.addCommand("RUN", wrapperCall_RUN);

  cmd.addCommand("STOP", wrapperCall_STOP);

  cmd.addCommand("VIBTIME", wrapperSet_VIBTIME);
  cmd.addCommand("SEEDINT", wrapperSet_SEEDINT);
  

}

//// Code 

void getInfo() {
  cmd.print("DephasedPWM,");
  cmd.println(COMPILE_DATE_TIME);
}

void unrecognized(const char *command) {
  error("Unknown command");
}
//// Auto generated Feat and DictFeat Code
// COMMAND: INITIALIZE, Action: initialize


void wrapperSet_FREQUENCY() {
  char *arg;
  
  arg = cmd.next();
  if (arg == NULL) {
    error("No value stated");
    return;
  }
  uint32_t frequency = atol(arg);
  if (frequency > 0) {
    set_FREQUENCY(frequency);
    ok();
  } else {
    error("invalid value, must be larger than 0");
  }
}

void wrapperSet_PHASE() {
  char *arg;
  
  arg = cmd.next();
  if (arg == NULL) {
    error("No value stated");
    return;
  }
  int phase = atoi(arg);
  if (phase >= 0 && phase <= 360) {
    set_PHASE(phase);
    ok();
  } else {
    error("invalid value, must be between 0 and 360");
  }
}

void wrapperSet_DELTA() {
  char *arg;
  
  arg = cmd.next();
  if (arg == NULL) {
    error("No value stated");
    return;
  }
  int delta = atoi(arg);
  if ((delta >= 1 && delta <= 45) || (delta >= -45 && delta <= -1)) {
    set_DELTA(delta);
    ok();
  } else {
    error("invalid value, must be between 1 and 45 (positive or negative)");
  }
}

void wrapperSet_TIMESTEP() {
  char *arg;
  
  arg = cmd.next();
  if (arg == NULL) {
    error("No value stated");
    return;
  }
  int timestep = atoi(arg);
  if (timestep >= 10 && timestep <= 100) {
    set_TIMESTEP(timestep);
    ok();
  } else {
    error("invalid value, must be between 10 and 100");
  }
}


//void wrapperCall_PAUSE() {
 // int err = call_PAUSE();
 // if (err == 0) {
 //   ok();
 // } else {
  //  error_i(err);
 // }
//};

void wrapperSet_VIBTIME() {
  char *arg;
  
  arg = cmd.next();
  if (arg == NULL) {
    error("No vibration time value stated");
    return;
  }
  int vibration_time = atoi(arg);
  int err = set_VIBTIME(vibration_time);
  if (err == 0) {
    ok();
  } 
}


void wrapperSet_SEEDINT() {
  char *arg;
  
  arg = cmd.next();
  if (arg == NULL) {
    error("No seed interval  value stated");
    return;
  }
  int vibration_time = atoi(arg);
  int err = set_SEEDINT(vibration_time);
  if (err == 0) {
    ok();
  }
}

void wrapperCall_RUN() {
  call_RUN();
  ok();
}

void wrapperCall_STOP() {
  call_STOP();
  ok();
}
  