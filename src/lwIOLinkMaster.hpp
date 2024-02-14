#pragma once
#include "port.hpp"
#include "driver/phy/phy_driver.hpp"
#include "driver/phy/tiol112/tiol112.hpp"

namespace lwIOLink {

class Master
{
    public:
    Master(Port ** port_list, uint8_t total_ports):
        m_port_list(port_list),
        m_total_ports(total_ports){};
    bool begin();
    private:
        Port ** m_port_list;
        const uint8_t m_total_ports;
};

} // namespace lwIOLink