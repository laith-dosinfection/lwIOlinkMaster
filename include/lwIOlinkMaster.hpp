#ifndef __LW_IO_LINK_MASTER_H__
#define __LW_IO_LINK_MASTER_H__

#include "utility/iolinkPort.hpp"

#if __has_include(<sdkconfig.h>)
#include <sdkconfig.h>
#endif

#include <vector>

/// global instance.
//IOlink::Master master;

namespace IOlink
{
    class Master
    {
    public:
        typedef enum
        {
            UART,
            I2C,
            SPI
        } PHY_comm_t;
        
        //typedef std::vector<Port>;

        Master(){};
        ~Master(){};

        void begin(){
            for (int i = 0; i < num_ports; i++)
            {
                //Port *p[i] = new Port();
            }
        };

        void update(){

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
    protected:
        int8_t num_ports = 1;
        PHY_comm_t interface = UART;
    };
};

#endif