#ifndef __IO_LINK_PORT_H__
#define __IO_LINK_PORT_H__

#if __has_include(<driver/uart.h> )

#include <driver/uart.h>
#include "lwIOlinkMaster.hpp"

#endif

#include <Arduino.h>
#include <cstdint>
#include <cstddef>
#include "sdci.hpp"

#define DATA_SIZE 1

namespace IOlink
{
  //Forward Declaration of port classes
  //SIO in is single input and output
  //TODO: implement SIOPort
  //class SIOPort;
  //SDCI stands for Single drop digital communication interface
  class SDCIPort;
  
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

    Port(Master* master) : master(master) {
      sdciPort = new SDCIPort();
      //sioPort = new SIOPort();
      configurePort();
    };
    ~Port() {
      delete sdciPort;
      //delete sioPort;
    };


    update(){
        //sioPort->update();
        sdciPort->update();
    }

    /// @brief need to dynamically configure the port based on its type if generic port gets created will need to destroy the port and create an SIO port or inherit port in SIO port
    void configurePort(){
      switch (mode)
      {
        //Need to implement other case statements and build the functions to back them up
        case IOL_AUTOSTART:
          sdciPort->
          break;
        default:
          break;
      }
    }

    /// @brief define the fuction of the port, set it as IO link/DI/DO
    /// @param mode enum of port modes
    void setPortMode(mode_t _mode)
    {
      mode = _mode;
      //configurePort();
    };
    /// @brief return the enum mode of the port
    /// @return enum mode_t DEACTIVATED, IOL_manual, IOL_auto, DI, DO
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
    mode_t mode = IOL_AUTOSTART;
    uint8_t port_num = 1;
  private:
    Master* master;
    //SIOPort* sioPort;
    SDCIPort* sdciPort;
  };
}

#endif
