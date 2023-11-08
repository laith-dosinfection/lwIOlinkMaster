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
  IOlink::Master = new IOlink::master();
  IOlink::Master->setNumPorts(1);
  IOlink::Master->setPHYInterface((IOlink::master::PHY_comm_t)IOlink::master::PHY_comm_t::UART);

  for(int i=0; i< IOlink::Master->getNumPorts(); i++){
    //construct the number of ports
    IOlink::Master port[i] = new IOlink::IOlinkPort::IOlinkPort();
  }

  for(int i=0; i< IOlink::Master->getNumPorts(); i++){
    IOlink::IOlinkPort->
  }





}

void loop(void)
{

}








void IO_link_read(){
  
  //if(Serial1.available()){
    //IDSU Start
    //Serial1.write(0xF1);
    //Serial1.write(0x94);
    //Serial1.write(0b11110000);
    //IDSU First
    //Serial1.write(0b11100001);
  //}
  //digitalWrite(ENABLE_PIN,LOW);

  //wake(WAKE_PIN, WAKE_PULSE);

  if(Serial1.available()){
    M5.Display.clear();
    M5.Display.setCursor(0,0);
    byte buff[4];

    digitalWrite(ENABLE_PIN,LOW);
    Serial1.write(0b11110000);
    digitalWrite(ENABLE_PIN,HIGH);
    
    buff[0] = Serial1.read();
    
    digitalWrite(ENABLE_PIN,LOW);
    Serial1.write(0b11110001);
    digitalWrite(ENABLE_PIN,HIGH);
    
    buff[1] = Serial1.read();
    
    digitalWrite(ENABLE_PIN,LOW);
    Serial1.write(0b11110010);
    digitalWrite(ENABLE_PIN,HIGH);
    

    buff[2] = Serial1.read();

    digitalWrite(ENABLE_PIN,LOW);
    Serial1.write(0b11110011);
    digitalWrite(ENABLE_PIN,HIGH);
    
    buff[3] = Serial1.read();
    
    for(int i = 0; i<4; i++){
      M5.Display.print(buff[i]);
      Serial.printf("index %d : %d \n", i, buff[i]);
    }
    
  }else{
    Serial.println("Serial Unavailable");
  }
}



// for ESP-IDF compat
#if !defined ( ARDUINO ) && defined ( ESP_PLATFORM )
extern "C" {
  void loopTask(void*)
  {
    setup();
    for (;;) {
      loop();
    }
    vTaskDelete(NULL);
  }

  void app_main()
  {
    xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, 1);
  }
}
#endif