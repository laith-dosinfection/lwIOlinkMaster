#include "iolinkPort.hpp"

namespace IOlink
{

class TIOL112 : public PhyDriver
{
public:
  /// @brief structure to define the relevant pins of an io link port
  struct pins_t// : public PhyDriver::Hw_cfg_t 
  {
    uint8_t wake_pin;//The pin number of the wake pin
    uint8_t EN_pin;//The pin number of the enable pin
    uint8_t Fault_pin;//The pin number of the fault pin
    Stream* serialPtr; 
  };

  TIOL112()
  {
    pinInit();
    establishComm();
  };
  ~TIOL112(){};


  uint8_t getRXPin()
  {
    return RX_pin;
  };
  void setRXPin(uint8_t RX)
  {
    RX_pin = RX;
  };

  uint8_t getTXPin()
  {
    return TX_pin;
  };
  void setTXPin(uint8_t TX)
  {
    TX_pin = TX;
  };

  void pinInit() override
  {
    if (pin_init)
    {
      pinMode(pins.EN_pin, OUTPUT);
      pinMode(pins.EN_pin, LOW);
      pinMode(pins.wake_pin, OUTPUT);
      digitalWrite(pins.wake_pin, LOW);
      pinMode(pins.Fault_pin, INPUT);
      //I don't know if you are allowed to do this?  Just trying to abrstract the serial port
      cfg.serialPtr->begin(rate, SERIAL_8E1, RX_pin, TX_pin);
      pin_init = true;
    }
  };

  /// @brief Method to test a specific ports comm rate
  /// @param rate pass through a transmission rate
  /// @return true if the tested com rate is true for the device
  bool testBaud(baud_t b) override
  {
    auto baud = (baud_t) b;
    // set the rates of relevant timing and speed variables based on speed
    SIOPort::setRates(baud);
    // UART Frames have an 8 bit even parity
    if (Serial.baudRate() != rate)
    {
      cfg.serialPtr->updateBaudRate(rate);
    }

    cfg.serialPtr->flush();

    uint8_t bytesReceived = 0;
    uint8_t dataSent[DATA_SIZE], dataReceived[DATA_SIZE];
    dataSent[DATA_SIZE-1] = {0b11111111};
    
    //size_t sentBytes = Serial.write(dataSent, sizeof(dataSent));
    size_t sentBytes = cfg.serialPtr->write(dataSent, sizeof(dataSent));
    unsigned long now = micros();
    bool timeout = false;
    Serial.available();
    while (bytesReceived < DATA_SIZE)
    {
      bytesReceived += cfg.serialPtr->read(dataReceived, DATA_SIZE);
      if (micros() - now > t_dmt_s)
      {
        timeout = true;
        break;
      }
    }
    return !timeout;
  };

protected:
  uint8_t RX_pin = 44;
  uint8_t TX_pin = 43;
  pins_t cfg = {.wake_pin = pins.wake_pin, .enable_pin = pins.enable_pin, .fault_pin = pins.fault_pin, .serialPtr =&Serial};
};
}
}