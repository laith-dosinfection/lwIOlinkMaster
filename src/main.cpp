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
  IOlink::Master.begin();
}

void loop(){
  IOlink::Master.handle();
}