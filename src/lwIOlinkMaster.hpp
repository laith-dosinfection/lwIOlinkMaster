#ifndef __LW_IO_LINK_MASTER_H__
#define __LW_IO_LINK_MASTER_H__

#include "utility/iolinkPort.hpp"

#if __has_include(<sdkconfig.h>)
#include <sdkconfig.h>
#endif

#include <vector>
//#include <TaskScheduler.h>

/// global instance.
//IOlink::Master master;

namespace IOlink
{
    class Master
    {
    public:
        
        /// @brief Master port IC definitions of the PHY IC used by the schematic
        enum SDCI_t
        {
            TIOL11X,
            UNKNOWN
        };

        enum PortSwitch_t
        {
            TPS27S100A, //TPS27S100A has a "Fault pin" which is driven low in the case of a fault 
            TPS27S100B, //TPS27S100B has a Current monitoring pin which outputs a voltage which maps to port current.
            NONE
        };

        /// @brief TODO: Future implementation of a SIO pin to be used.
        enum SIO_t
        {
            NONE
        };

        struct PortConfig
        {
            SDCI_t SDCI;
            SIO_t SIO;
            PortSwitch_t Switch;
        };
        
        
        template<typename T>
        T* createPort() {
            T* portObj = new T(this); // Create a new instance of the port class
            ports.push_back(portObj); // Store the pointer in a collection
            return portObj;
        };

        void begin(PortConfig _cfg){
            for (int i = 0; i < num_ports; i++)
            {
                this->createPort();
            }
        };

        void begin(){
            begin((PortConfig) {.SDCI = TIOL11X, .SIO = NONE, .Switch = TPS27S100B});
        };

        void update(){
            for (auto& portObj : ports) {
                portObj->update();
            }
        };

        int8_t getNumPorts()
        {
            return num_ports;
        };
        void setNumPorts(uint8_t _num_ports)
        {
            num_ports = _num_ports;
        };

        void setCfg(PortConfig _cfg){
            cfg = _cfg;
        };

        PortConfig getCfg(){
            return cfg;
        };


    protected:
        ///TODO: Will need to programatically set the max number of ports based on the PHY & the processor's I/O
        int8_t max_num_ports = 2;
        int8_t num_ports = 1; //Just going to test with a single port for now.
        std::vector<class Port*> ports;
        PortConfig cfg;
    };
};

#endif