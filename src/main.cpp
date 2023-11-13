#if defined ( ARDUINO )

#include <Arduino.h>

// If you use SD card, write this.
#include <SD.h>

// If you use SPIFFS, write this.
#include <SPIFFS.h>

#endif

#include "lwIOlinkMaster.h"

IOlink::UARTSIO port1;

void setup(void)
{
  //master.begin();
}

void loop(){
  //master.update();
}