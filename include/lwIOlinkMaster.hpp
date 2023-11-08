#ifndef __LW_IO_LINK_MASTER_HPP__
#define __LW_IO_LINK_MASTER_HPP__

#include "utility/iolinkPort.hpp"

#if __has_include(<sdkconfig.h>)
#include <sdkconfig.h>
#endif

namespace IOlink
{
   class master
   {
        protected:
        enum PHY_comm_t
        {
            UART,
            I2C,
            SPI
        };
        int8_t num_ports;
        PHY_comm_t interface;
        public:
        
        master(){};
        ~master(){};

        int8_t getNumPorts()
        {
            return num_ports;
        };
        void setNumPorts(uint8_t _num_ports)
        {
            num_ports = _num_ports;
        };

        PHY_comm_t getPHYInterface()
        {
            return interface;
        };
        void setPHYInterface(PHY_comm_t _interface)
        {
            interface = _interface;
        };
   };

   master *Master;
};

#endif