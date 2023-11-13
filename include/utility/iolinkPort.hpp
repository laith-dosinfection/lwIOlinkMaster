#ifndef __IO_LINK_PORT_H__
#define __IO_LINK_PORT_H__

#if __has_include(<driver/uart.h> )

#include <driver/uart.h>
#include "lwIOlinkMaster.hpp"

#endif

#include <Arduino.h>
#include <cstdint>
#include <cstddef>

namespace IOlink
{

  class Port
  {
  public:
    typedef enum
    {
      DEACTIVATED,
      IOL_MANUAL,
      IOL_AUTOSTART,
      DI_CQ,
      DO_CQ
    } mode_t;

    Port(int n, mode_t m)
    {
      setPortNum(n);
      switch (m)
      {
      case IOL_AUTOSTART
      SIOComm(n);
          break;

          default:
        break;
      }
    };
    ~Port() { delete (mst); };

    /// @brief define the fuction of the port, set it as IO link/DI/DO
    /// @param port_mode enum of port modes
    void setPortMode(mode_t _mode)
    {
      mode = _mode;
    };
    /// @brief return the enum mode of the port
    /// @return enum port_mode_t DEACTIVATED, IOL_manual, IOL_auto, DI, DO
    mode_t getPortMode()
    {
      return mode;
    };

    void setPortNum(int n)
    {
      port_num = n;
    };

    uint8_t getPortNum()
    {
      return port_num;
    }

  protected:
    mode_t mode = DEACTIVATED;
    int port_num;
  };

  // SIO should inherit a port pointer
  class SIOComm
  {
  public:
    typedef enum
    {
      NOT_DETECTED,
      COM1,
      COM2,
      COM3
    } baud_t;

    typedef enum
    {
      IDLE,
      WAKE,
      ESTABLISHING_COMM,
      SIO,
      SDCI
    } state_t;

    /// @brief Wake-Up paremeters are IO link master stack constants for establishing communication
    /// with an IO link slave device for a given IO link master port
    const union wake_parameters_t
    {
      uint8_t t_wu : 80;    // the wake up request pulse time, minimum time is 75uS, maximum time is 85uS
      uint16_t t_ren : 500; // the maximum ammount of time an IO-link device can take to configure itself for communicaion (500uS)
      uint8_t n_wu : 2;     // the maximum number of retries to establish communication
      uint8_t t_dwu : 50;   // wake-up retry delay, max is 50ms min is 30ms (measured in ms).
      uint8_t t_sd : 1;     // Time between two wake up requests, max is 1S minimum time is 500ms.
      uint8_t t_dmt : 37;   // Master message delay time, max is 37 and min is 27 (measured in Tbit).
      uint8_t t_dsio : 300; // Standard IO Delay, min 60, max 300 (measured in ms)
      struct speeds_t
      {
        uint32_t com1 : 4800;
        uint32_t com2 : 38400;
        uint32_t com3 : 230400;
      } speeds;
    } wake;

    // TODO: should I be passing PHY comm through and create comm pins in a inherited item or redefine the structure of pins_t in sequential classes?
    typedef struct
    {
      // PHY_comm_t comm;
      uint16_t wake_pin;
      uint16_t EN_pin;
      uint16_t Fault_pin;
    } pins_t;

    SIOComm()
    {
      pinInit();
      enable();
    };
    ~SIOComm();

    baud_t getBaud()
    {
      return baud;
    };
    void setBaud(baud_t _baud)
    {
      baud = _baud;
    };
    uint32_t rateEnumtoComm(baud_t b)
    {
      uint32_t r = 0;
      switch (b)
      {
      case NOT_DETECTED:
        break;
      case COM1:
        r = wake.speeds.com1;
        break;
      case COM2:
        r = wake.speeds.com2;
        break;
      case COM3:
        r = wake.speeds.com3;
        break;
      default:
        break;
      }
      return r;
    }

    /// @brief Allowable Data Rates for communication of IO-link devices
    /// The master sends multiple messages at the COM3, COM2, and COM1 data rates (fastest to slowest)
    /// and waits for the device to respond after each send to determine the devices comm speed (baud).
    uint32_t getRate(baud_t b)
    {
      return rateEnumtoComm(b);
    }

    uint32_t getRate()
    {
      return rate;
    }

    void setRate(uint32_t r)
    {
      rate = r;
    }

    void setRate(baud_t b)
    {
      rate = rateEnumtoComm(b);
    }

    uint32_t getBitRate()
    {
      return bitRate;
    }

    void setBitRate(baud_t b)
    {
      uint32_t r = rateEnumtoComm(b);
      setBitRate(r);
    }

    void setBitRate(uint32_t br)
    {
      bitRate = br;
    }

    uint8_t getDelayTime()
    {
      return t_dmt_s;
    }

    void setDelayTime(uint8_t t)
    {
      t_dmt_s = t;
    }

    void setRates(baud_t b)
    {
      setBaud(b);
      uint32_t r = rateEnumtoComm(b);
      try
      {
        if (r == 0)
          throw -1;
        setRate(r);
        setBitRate((uint8_t)(1000000 / r));
        setDelayTime((uint8_t)(1000000 * wake.t_dmt / r));
      }
      catch (int x)
      {
      }
    }

    wake_parameters_t getWakeParameters()
    {
      return wake;
    };

    pins_t getPins()
    {
      return pins;
    }

    void setPins(pins_t p)
    {
      pins = p;
    }

    bool wakeUpRequest()
    {
      bool wakeSuccess = false;
      if (millis() - lastWake > wake.t_dwu)
      {
        digitalWrite(this->pins.wake_pin, HIGH);
        delayMicroseconds(this->wake.t_wu);
        digitalWrite(this->pins.wake_pin, LOW);
        delayMicroseconds(this->wake.t_wu);
        lastWake = millis();
        wakeSuccess = true;
      }
      return wakeSuccess;
    };

    virtual void pinInit();
    virtual bool testBaud(baud_t b);

    void enable()
    {
      if (Enable)
      {
        digitalWrite(pins.EN_pin, HIGH);
        Enable = true;
      }
    };
    void disable()
    {
      if (!Enable)
      {
        digitalWrite(pins.EN_pin, LOW);
        Enable = false;
      }
    };
    baud_t getComSpeed()
    {
      baud_t testing_baud = NOT_DETECTED;
      if (testBaud(COM3))
      {
        testing_baud = COM3;
      }
      else if (testBaud(COM2))
      {
        testing_baud = COM2;
      }
      else if (testBaud(COM1))
      {
        testing_baud = COM1;
      }
      return testing_baud;
    }

    bool establishComm()
    {
      baud_t comm_speed;
      uint8_t wake_attempts = 0;
      last_WUR = millis();

      if (millis() - last_WUR > wake.t_sd)
      {
        while (comm_speed == NOT_DETECTED && wake_attempts <= wake.n_wu)
        {
          if (wakeUpRequest())
          {
            comm_speed = getComSpeed();
            wake_attempts++;
          }
        }
        if (comm_speed != NOT_DETECTED)
        {
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }

  protected:
    pins_t pins = {
      wake_pin : 36;
    EN_pin : 37;
    Fault_pin : 38;
  };
  bool pin_init = false;
  bool Enable = false;
  uint32_t lastWake;
  uint32_t last_WUR;

  state_t state = IDLE;
  baud_t baud = NOT_DETECTED;       // baud rate as an enum
  uint32_t rate = wake.speeds.com3; // the baud rate of comm
  uint8_t bitRate = 0;              // the bitrate of comm in uS
  uint8_t t_dmt_s = 0;              // Actual time of delay
};

class UARTSIO : public SIOComm
{
public:
  UARTSIO()
  {
    establishComm();
  };
  ~UARTSIO(){};

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

  void pinInit()
  {
    if (pin_init)
    {
      pinMode(pins.EN_pin, OUTPUT);
      pinMode(pins.EN_pin, LOW);
      pinMode(pins.wake_pin, OUTPUT);
      digitalWrite(pins.wake_pin, LOW);
      pinMode(pins.Fault_pin, INPUT);
      Serial.begin(rate, SERIAL_8E1, RX_pin, TX_pin);
      pin_init = true;
    }
  };

  /// @brief Method to test a specific ports comm rate
  /// @param rate pass through a transmission rate
  /// @return true if the tested com rate is true for the device
  bool testBaud(baud_t b)
  {
    // set the rates of relevant timing and speed variables based on speed
    setRates(b);
    // UART Frames have an 8 bit even parity
    if (Serial.baudRate() != rate)
    {
      // Serial.begin(rate, SERIAL_8E1, RX_pin, TX_pin);
      Serial.updateBaudRate(rate);
    }

    Serial.flush();

    uint8_t dataReceived, bytesReceived;

    size_t sentBytes = Serial.write(0b11111111);
    unsigned long now = micros();
    bool timeout = false;
    Serial.available();
    while (bytesReceived < 1)
    {
      bytesReceived += Serial.read(dataReceived);
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
};

// TODO: Should be able to inherit mutliple PHY interface types
class IODeviceComm
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
    // R/W bit
    enum r_w_t
    {
      write,
      read
    };

    // Communication Channel
    enum comm_channel_t
    {
      process,
      page,
      diagnosis,
      ISDU
    };

    // Address
    //  Other 5 bits
  };
};
}

#endif
