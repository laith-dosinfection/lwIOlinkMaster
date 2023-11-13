#if defined ( ARDUINO )

#include <Arduino.h>

// If you use SD card, write this.
#include <SD.h>

// If you use SPIFFS, write this.
#include <SPIFFS.h>

#endif

#include "lwIOlinkMaster.h"


void setup(void)
{
  shared_ptr<master> m(new IOlink::Master());
  
  IOlink::Master.begin();
}

void loop(){
  IOlink::Master.handle();
}