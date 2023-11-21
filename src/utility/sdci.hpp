#ifdef ARDUINO_ARCH_ESP32
#include "driver/uart.h"
#include "soc/uart_reg.h"
#endif	//ARDUINO_ARCH_ESP32


namespace IOlink
{
    class SDCIPort
    {
    
    /// @brief Structure for the various PHY SDCI chips
    struct PHY_t
    {
        //Type of Protocols for the various PHY chips
        enum PHY_Comm_t
        {
            UART,
            SPI,
            I2C
        };
        PHY_Comm_t comm;

        /// @brief PHY EN Pin, allow definition of no EN pin, direct pin or from a gpio expander
        enum PHY_EN_t
        {
            NONE,
            BUILTIN
        };
        PHY_EN_t en;

        /// @brief Fault pin definition
        enum Fault_t
        {
            NONE,//No Used Fault pin
            BUILTIN_LOGIC_LOW,//Fault is logic level low (0 is considered a fault)
            BUILTIN_LOGIC_HIGH //Fault is logic level high
        };
        Fault_t fault;

        int8_t numSDCIPorts;//I am not 100% sure how I want to handle ICs which have more than 1 IO-link ports
    };

    /// @brief IO-link Baud Rates
    enum BaudRate: uint32_t
    {
        COM1 = 4800,
        COM2 = 38400,
        COM3 = 230400,
        NOT_DETECTED = 0
    };

    /// @brief Wake-Up paremeters are IO link master stack constants for establishing communication
    /// with an IO link slave device for a given IO link master port
    enum WakeParameters: uint16_t
    {
        t_wu = 80, // the wake up request pulse time, minimum time is 75uS, maximum time is 85uS
        t_ren = 500, // the maximum ammount of time an IO-link device can take to configure itself for communicaion (500uS)
        n_wu = 2, // the maximum number of retries to establish communication
        t_dwu = 50, // wake-up retry delay, max is 50ms min is 30ms (measured in ms).
        t_sd = 1, // Time between two wake up requests, max is 1S minimum time is 500ms.
        t_dmt = 37, // Master message delay time, max is 37 and min is 27 (measured in Tbit).
        t_dsio = 300 // Standard IO Delay, min 60, max 300 (measured in ms)
    };
    
    /// @brief structure to define the relevant pins of an io link port
    struct HWConfig// : public PhyDriver::Hw_cfg_t 
    {
        Stream &serialPtr;
        struct PHY_Pin_t
        {
            unsigned EN;    //The pin number of the enable pin
            unsigned Fault; //The pin number of the fault pin
            unsigned Tx;	//TX pin #
            unsigned Rx;    //RX pin #
        };
        PHY_Pin_t PHY_Pins;
        struct Power_Pin_t
        {
            unsigned EN;
            unsigned IMON;
        };
        Power_Pin_t Power_Pins; 
    };

    bool PHYEnable;
    bool PowerEnable;
    uint16_t current;

    public:
    
    SDCIPort()
    {
      begin();
      establishComm();
      enable();
      initComm();
    };
    ~SDCIPort(){};

    void begin(){
        switch (master->getCfg().SDCI)
        {
        case SDCI_t::TIOL11X:
            setPHY((PHY_t) {.comm = PHY_Comm_t::UART, .en = PHY_EN_t::BUILTIN, .fault = PHY_Fault_t::BUILTIN_LOGIC_LOW, .numSDCIPorts = 1});
            TIOL11X_init(this->getPortNum());
            break;
        default:
            break;
        };
        pinInit();
    };

    void TIOL11X_init(uint8_t _port_num){
        //Some hardcoded pin mappings for the dev board
        if(_port_num == 1){
            setConfig((HWConfig) {.serialPtr = Serial, .PHY_Pins={.EN = 42, .Fault = 41, .Rx = 44, .Tx = 43}, .Power_Pins={.EN = 2, .IMON = 1}});
        } else if (_port_num == 2){
            setConfig((HWConfig) {.serialPtr = Serial1, .PHY_Pins={.EN = 21, .Fault = 22, .Rx = 18, .Tx = 17}, .Power_Pins={.EN = 20, .IMON = 19}});
        }
    };

    /// @brief Set the parameters of the PHY port
    void setPHY(PHY_t _PHY){
        PHY = _PHY;
    };

    PHY_t getPHY(){
        return PHY;
    };

    /// @brief setting the config of the SDCI port
    void setConfig(HWConfig _config){
        config = _config;
    };

    HWConfig getConfig(){
        return config;
    };

    /// @brief Do some stuff to handle fault events
    void phyFault(){

    };

    void pinInit()
    {
        if (pin_init)
        {
            switch ((PHY_EN_t)this->PHY.en)
            {
            case BUILTIN_LOGIC_LOW:
                pinMode(config.PHY_Pins.EN, OUTPUT);
                pinMode(config.PHY_Pins.EN, LOW);
                break;
            default:
                break;
            };

            switch ((Fault_t)this->PHY.fault)
            {
            case BUILTIN_LOGIC_LOW:
                pinMode(config.PHY_Pins.Fault, INPUT);
                //Playing around with the idea of tying to an interrupt, might just check pin level periodically instead.
                attachInterrupt(digitalPinToInterrupt(config.PHY_Pins.Fault), phyFault, FALLING);
                break;
            default:
                break;
            };
            
            //I don't know if you are allowed to do this?  Just trying to abrstract the serial port
            static_cast<HardwareSerial*>(SerialPort)->begin(static_cast<uint32_t> (Baud), SERIAL_8E1, config.PHY_Pins.Rx, config.PHY_Pins.Tx);
            pin_init = true;
        }
    };

    void initComm(){
        determineDeviceBaud();
    };

    BaudRate determineDeviceBaud(){
        wakeUpRequest();
    };

    void Update(){
        
    };

    BaudRate getBaud()
    {
      return Baud;
    };
    void setBaud(BaudRate _Baud)
    {
      Baud = _Baud;
    };

    /// @brief Allowable Data Rates for communication of IO-link devices
    /// The master sends multiple messages at the COM3, COM2, and COM1 data rates (fastest to slowest)
    /// and waits for the device to respond after each send to determine the devices comm speed (baud).
    uint32_t getRate()
    {
      return (uint32_t)this->Baud;
    };

    uint32_t getBitRate()
    {
      return bitRate;
    };

    void setBitRate(baud_t b)
    {
      uint32_t r = rateEnumtoComm(b);
      setBitRate(r);
    };

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

    HWConfig getConfig()
    {
      return config;
    }

    void setConfig(HWConfig cfg)
    {
      config = cfg;
    }

    bool wakeUpRequest()
    {
      bool wakeSuccess = false;
      if (millis() - lastWake > WakeParameters::t_dwu)
      {
        digitalWrite(config.PHY_pins.TX , HIGH);
        delayMiroseconds(WakeParameters::t_wu);
        digitalWrite(config.PHY_pins.TX, LOW);
        delayMicroseconds(WakeParameters::t_wu);
        lastWake = millis();
        wakeSuccess = true;
      }
      return wakeSuccess;
    };

    virtual bool testBaud(baud_t b) = 0;

    /// @brief Enable both PHY IC and PWR
    void enable(){
        enablePHY();
        enablePWR();
    }

    /// @brief Drive the enable pin for the PHY chip High
    void enablePHY()
    {
      if (PHYEnable)
      {
        digitalWrite(config.PHY_pins.EN, HIGH);
        PHYEnable = true;
      }
    };

    /// @brief Drive the enable pin for the PHY chip Low
    void disablePHY()
    {
      if (!PHYEnable)
      {
        digitalWrite(config.PHY_pins.EN, LOW);
        PHYEnable = false;
      }
    };

    /// @brief Drive enable pin for smart side switch high
    void enablePWR()
    {
      if (PowerEnable)
      {
        digitalWrite(config.Power_Pins.EN, HIGH);
        PowerEnable = true;
      }
    };

    /// @brief Drive the enable pin for the smart side switch Low
    void disablePWR()
    {
      if (!PowerEnable)
      {
        digitalWrite(config.Power_Pins.EN, LOW);
        PowerEnable = false;
      }
    };


    bool establishComm()
    {
      uint8_t wake_attempts = 0;
      last_WUR = millis();

      if (millis() - last_WUR > wake.t_sd)
      {
        while (Baud == NOT_DETECTED && wake_attempts <= wake.n_wu)
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
        PHY_t PHY;
        HWConfig config;
        bool pin_init = false;
        bool PHYEnable = false;
        bool PowerEnable = false;
        uint32_t lastWake;
        uint32_t last_WUR;

        //state_t state = IDLE;
        BaudRate Baud = NOT_DETECTED;			// baud rate for transmission
        uint8_t bitRate = 0;              // the bitrate of comm in uS
        uint8_t t_dmt_s = 0;              // Actual time of delay
    };  
}