#ifndef __LW_IO_LINK_MASTER_H__
#define __LW_IO_LINK_MASTER_H__

#include "utility/iolinkPort.hpp"

#if __has_include(<sdkconfig.h>)
#include <sdkconfig.h>
#endif

namespace IOlink
{
    typedef enum
    {
        UART,
        I2C,
        SPI
    } PHY_comm_t;

    class master
    {
    protected:
        int8_t num_ports = 1;
        PHY_comm_t interface = UART;

    public:
        master()
        {
            for (int i = 0; i < num_ports; i++)
            {
                Port *p[i] = new Port(master * Master);
            }
        };
        ~master(){};

        void begin(){

        };

        void handle(){

        };

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