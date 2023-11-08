#ifndef __IO_LINK_PORT_H__
#define __IO_LINK_PORT_H__

#include "../src/utility/m5unified_common.h"
#if __has_include ( <driver/uart.h> )

#include <driver/uart.h>

#endif

#include <Arduino.h>
#include <cstdint>
#include <cstddef>

namespace IOlink
{
  class Port: public master 
  {
      protected:
      enum mode_t
      {
          DEACTIVATED,
          IOL_MANUAL,
          IOL_AUTOSTART,
          DI_CQ,
          DO_CQ 
      };
      mode_t mode = DEACTIVATED;
      
      public:
      Port(){};
      ~Port(){};
      
      /// @brief define the fuction of the port, set it as IO link/DI/DO
      /// @param port_mode enum of port modes
      void setPortMode(mode_t _mode){
        mode = _mode;
      };
      /// @brief return the enum mode of the port
      /// @return enum port_mode_t DEACTIVATED, IOL_manual, IOL_auto, DI, DO
      mode_t getPortMode(){
        return mode;
      };      
  };

  class SIOComm :public Port
  {
    protected:
    enum baud_t{
      NOT_DETECTED,
      COM1,
      COM2,
      COM3
    };
    baud_t baud = NOT_DETECTED; //baud rate as an enum
    uint32_t rate = 0; //the baud rate of comm
    uint8_t bitRate = 0; //the bitrate of comm in uS
    uint8_t t_dmt_s = 0;//Actual time of delay

    enum state_t
    {
      IDLE,
      WAKE,
      ESTABLISHING_COMM,
      SIO,
      SDCI
    };
    state_t state = IDLE;

    public:
    SIOComm();
    ~SIOComm();

    /// @brief Wake-Up paremeters are IO link master stack constants for establishing communication
    /// with an IO link slave device for a given IO link master port
    const union wake_parameters_t
    {
      uint8_t t_wu : 80; //the wake up request pulse time, minimum time is 75uS, maximum time is 85uS
      uint16_t t_ren : 500; //the maximum ammount of time an IO-link device can take to configure itself for communicaion (500uS)
      uint8_t n_wu : 2; //the maximum number of retries to establish communication
      uint8_t t_dwu :  50; //wake-up retry delay, max is 50ms min is 30ms (measured in ms).
      uint8_t t_sd : 1; //Time between two wake up requests, max is 1S minimum time is 500ms.
      uint8_t t_dmt : 37; //Master message delay time, max is 37 and min is 27 (measured in Tbit).
      uint8_t t_dsio : 300; //Standard IO Delay, min 60, max 300 (measured in ms)
      union speeds_t
      {
        uint32_t com1 = 4800;
        uint32_t com2 = 38400;
        uint32_t com3 = 230400;
      }speeds;
    }wake;


    baud_t getBaud(){
      return baud;
    };
    void setBaud(baud_t _baud){
      baud = _baud;
    };

    uint32_t rateEnumtoComm(baud_t _baud)
    {
      uint32_t _rate = 0;
      switch (_baud)
      {
        case NOT_DETECTED:
        break;
        case COM1:
        _rate = wake.speeds.com1;
        break;
        case COM2:
        _rate = wake.speeds.com2;
        break;
        case COM3:
        _rate = wake.speeds.com3;
        break;
        default:
        break;
      }
      return _rate;
    }

    /// @brief Allowable Data Rates for communication of IO-link devices
    /// The master sends multiple messages at the COM3, COM2, and COM1 data rates (fastest to slowest)
    /// and waits for the device to respond after each send to determine the devices comm speed (baud).
    uint32_t getRate(baud_t _baud){
      return rateEnumtoComm(_baud);
    }

    uint32_t getRate()
    {
      return rate;
    }

    void setRate(uint32_t _rate){
      rate = _rate;
    }

    void setRate(baud_t _baud){
      rate = rateEnumtoComm(_baud);
    }

    uint32_t getBitRate(){
      return bitRate;
    }

    void setBitRate(baud_t _baud)
    {
      uint32_t _rate = rateEnumtoComm(_baud);
      setBitRate(_rate);
    }

    void setBitRate(uint32_t _bitRate){
      bitRate = _bitRate;
    }

    uint8_t getDelayTime()
    {
      return t_dmt_s;
    }

    void setDelayTime(uint8_t _t_dmt_s){
      t_dmt_s = _t_dmt_s;
    }

    void setRates(baud_t _baud)
    {
      setBaud(_baud);
      uint32_t _rate = rateEnumtoComm(_baud);
      if(0 != _rate)
      {
        setRate(_rate);
        setBitRate((uint8_t)(1000000/_rate));
        setDelayTime((uint8_t)(1000000*wake.t_dmt/_rate));
      }
    }
   
    
    wake_parameters_t getWakeParameters(){
      return wake;
    };
  };

  class UARTDevice : public SIOComm
  {
    
    protected:
    //Hard coded some ESP32S3 pins for testing
    //TODO: make this logic more sophisticated to configure pins dynamically based on some parameters
    uint8_t RX_pin = 44;
    uint8_t TX_pin = 43;
    uint8_t wake_pin = 36;
    uint8_t EN_pin = 37;
    uint8_t Fault_pin = 38;
    bool pin_init = false;
    bool Enable = false;
    uint32_t lastWake;
    uint32_t last_WUR;
    
    public:
    UARTDevice(){
      pinInit();
      enable();
      establishComm();
    };
    ~UARTDevice(){};

    uint8_t getRXPin()
    {
      return RX_pin;
    };
    void setRXPin(uint8_t _RX_pin)
    {
      RX_pin = _RX_pin;
    };

    uint8_t getTXPin()
    {
      return TX_pin;
    };
    void setTXPin(uint8_t _TX_pin)
    {
      TX_pin = _TX_pin;
    };
    uint8_t getWakePin()
    {
      return wake_pin;
    };
    void setWakePin(uint8_t _wake_pin){
      wake_pin = _wake_pin;
    };

    uint8_t getENPin()
    {
      return EN_pin;
    };
    void setENPin(uint8_t _EN_pin)
    {
      EN_pin = _EN_pin;
    };

    /// @brief Method to get Ports pin numer
    /// @return port pin number
    uint8_t getFaultPin();
    /// @brief Configure the fault pin
    /// @param pin pass through the number of the pin to configure it as the fault pin
    void setFaultPin(uint8_t pin);

    bool wakeUpRequest()
    {
      bool wakeSuccess = false;
      if(millis()-lastWake > wake.t_dwu)
      {
        digitalWrite(getWakePin() , HIGH);
        delayMicroseconds(this->wake.t_wu);
        digitalWrite(getWakePin(), LOW);
        delayMicroseconds(this->wake.t_wu);
        lastWake = millis();
        wakeSuccess = true;
      }
      return wakeSuccess;
    };
    void pinInit()
    {
      if(pin_init)
      {
        pinMode(getENPin(), OUTPUT);
        pinMode(getENPin(), LOW);
        pinMode(getWakePin(), OUTPUT);
        digitalWrite(getWakePin(), LOW);
        pinMode(getFaultPin(), INPUT);
        pin_init = true;
      }
    };
    void enable()
    {
      if(Enable){
        digitalWrite(getENPin(), HIGH);
        Enable = true;
      }
    };
    void disable()
    {
      if(!Enable){
        digitalWrite(getENPin(), LOW);
        Enable = false;
      }
    };

    /// @brief Method to test a specific ports comm rate
    /// @param rate pass through a transmission rate
    /// @return true if the tested com rate is true for the device
    bool testBaud(baud_t _baud){
      //set the rates of relevant timing and speed variables based on speed
      setRates(this._baud);
      //UART Frames have an 8 bit even parity
      Serial.begin(rate, SERIAL_8E1, RX_pin, TX_pin);

      size_t sentBytes = Serial.write(0b11111111);
      unsigned long now = micros();
      bool timeout = false;
      while(bytesReceived < 1){
        bytesReceived += Serial.read(dataReceived, 1);
        if (micros() - now > t_dmt_s) {
          timeout = true;
          break;
        }
      }
      return !timeout;
    };

    baud_t getComSpeed(){
      baud_t testing_baud = NOT_DETECTED;
      if(testBaud(COM3)){
        testing_baud = COM3;
      }else if(testBaud(COM2)){
        testing_baud = COM2;
      }else if(testBaud(COM1)){
        testing_baud = COM1;
      }
      return testing_baud;
    }

    bool establishComm()
    {
      baud_t comm_speed;
      uint8_t wake_attempts = 0;
      last_WUR = millis();

      if(millis() - last_WUR > wake.t_sd)
      {
        while(comm_speed == NOT_DETECTED && wake_attempts <= wake.n_wu){
          if(wakeUpRequest())
          {
            comm_speed = getComSpeed();
            wake_attempts++;
          }
        }
        if(comm_speed != NOT_DETECTED){
          return true;
        } else {
          return false;
        }
      }else{
        return false;
      }
      
    }

    /// @brief An initial configuration of the IO link PHY and the device via the UART port within the function the enable pin is driven high,
    /// the wake pulse is sent and the appropriate timeout is waited
    /// com3 is tested first, then com2 and finally com1 to set the transmission speed
    /// @param UART_RX_pin RX pin
    /// @param UART_TX_pin TX pin
    /// @return 
    //bool configureUARTPort(uint8_t UART_RX_pin,uint8_t UART_TX_pin);

    

    
  };

  //TODO: Should be able to inherit mutliple PHY interface types
  class IODeviceComm : public UARTDevice
  {
    public:
    IODeviceComm(){};
    ~IODeviceComm(){};

    

    private:

    /// @brief M Sequence control
    /// @details M-Sequence Control octets are constructed from a packet consisting of:
    /// Bit 7: R/W bit
    /// Bit 6-5: Communication Channel
    /// Bit 0-4:  Address
    /// All the multi-octet data types shall be transmitted as a big-endian sequence, i.e. the most significant octet (MSO)
    struct m_sequence
    {
        //R/W bit
        enum r_w_t
        {
        write,
        read
        };

        //Communication Channel
        enum comm_channel_t
        {
        process,
        page,
        diagnosis,
        ISDU
        };

        //Address
        // Other 5 bits

    };
  }

}

#endif
