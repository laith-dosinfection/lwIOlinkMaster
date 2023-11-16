#include "uartPHY.hpp"


namespace IOlink
{
    class SDCIPort
    {
    public:
    typedef enum
    {
      NOT_DETECTED,
      COM1,
      COM2,
      COM3
    } baud_t;
    
    /// @brief Wake-Up paremeters are IO link master stack constants for establishing communication
    /// with an IO link slave device for a given IO link master port
    typedef struct 
    {
      const uint16_t t_wu;    // the wake up request pulse time, minimum time is 75uS, maximum time is 85uS
      const uint16_t t_ren; // the maximum ammount of time an IO-link device can take to configure itself for communicaion (500uS)
      const uint8_t n_wu;     // the maximum number of retries to establish communication
      const uint16_t t_dwu;   // wake-up retry delay, max is 50ms min is 30ms (measured in ms).
      const uint8_t t_sd;     // Time between two wake up requests, max is 1S minimum time is 500ms.
      const uint16_t t_dmt;   // Master message delay time, max is 37 and min is 27 (measured in Tbit).
      const uint16_t t_dsio; // Standard IO Delay, min 60, max 300 (measured in ms)
    } wake_parameters_t;

    typedef struct
    {
      uint32_t com1;
      uint32_t com2;
      uint32_t com3;
    } speeds_t;


    SDCIPort()
    {
      enable();
    };
    ~SDCIPort(){};

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
        r = speeds.com1;
        break;
      case COM2:
        r = speeds.com2;
        break;
      case COM3:
        r = speeds.com3;
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
        if (r == 0) throw -1;
        setRate(r);
        setBitRate((uint8_t)(1000000 / r));
        setDelayTime((uint8_t)(1000000 * wake.t_dmt / r));
      }
      catch (int x)
      {
      }
    }

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
        delayMicroseconds(wake.t_wu);
        digitalWrite(this->pins.wake_pin, LOW);
        delayMicroseconds(wake.t_wu);
        lastWake = millis();
        wakeSuccess = true;
      }
      return wakeSuccess;
    };

    virtual void pinInit() = 0;
    virtual bool testBaud(baud_t b) = 0;

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
        pins_t pins = {36, 37, 38};
        bool pin_init = false;
        bool Enable = false;
        uint32_t lastWake;
        uint32_t last_WUR;

        //state_t state = IDLE;
        baud_t baud = NOT_DETECTED;       // baud rate as an enum
        uint32_t rate = speeds.com3;      // the baud rate of comm
        uint8_t bitRate = 0;              // the bitrate of comm in uS
        uint8_t t_dmt_s = 0;              // Actual time of delay
    private:
        wake_parameters_t wake = {80,500,2,50,1,37,300};
        speeds_t speeds = {4800,38400,230400};
    };  
}