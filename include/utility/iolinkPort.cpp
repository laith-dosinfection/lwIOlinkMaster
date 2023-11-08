#include "iolinkPort.hpp"
#include <Wire.h>

IOlink::master::wake_parameters_t IOlink::master::getWakeParameters(){

  const uint8_t wake_pin_width = this->getWakeParameters().t_wu;
  IOlink::master::wake_parameters_t wake_pulse_width = this->t_wu
}

void IOlink::IOlinkPort::wake(){
  
}

bool IOlink::IOlinkPort::configureUARTPort(uint8_t UART_RX_pin,uint8_t UART_TX_pin,uint8_t Wake_pin,uint8_t EN_pin,uint8_t Fault_pin){

  Serial1.begin(dataRate.com3, SERIAL_8E1, UART_RX_pin, UART_TX_pin);
}