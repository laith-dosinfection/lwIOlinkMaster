#include "iolinkPort.hpp"

namespace IOlink
{

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
  pins_t cfg = {.enable_pin = pins.enable_pin, .fault_pin = pins.fault_pin, .serialPtr =&Serial};
};
}
}