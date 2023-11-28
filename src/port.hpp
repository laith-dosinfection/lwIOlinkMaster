#pragma once

#include "driver/phy/phy_driver.hpp"
#include "driver/highside/highside_driver.hpp"
#include "driver/current_monitor/current_monitor.hpp"

namespace lwIOLink
{
class Port {
public:
    /**
     * @brief Construct a new Port object
     * 
     * @param phyDriver Phy driver used for the Port
     * @param phy_port Port number assigned relative to the Phy
     * @param highSideDriver Optional highside driver if Phy does not implement it
     * @param currentDriver Optional current monitor driver if Phy does not implement it
     */
    Port(driver::Phy * phyDriver,
        const uint8_t phy_port,
        driver::HighSide * highSideDriver = nullptr,
        driver::CurrentMonitor* currentDriver = nullptr)
        :   m_phyDriver(phyDriver),
            m_phy_port(phy_port),
            m_HighSideDriver(highSideDriver),
            m_currentMonitorDriver(currentDriver) {};
    /**
     * @brief Set Port as a digital output 
     *          Typically used when the port is in SIO Mode
     * 
     * @param val Boolean value to set Port output to low or high
     * @return true if operation succeeded
     */
    bool setDO(bool val);
    /**
     * @brief Get the actual current consumption of the port
     * 
     * @param val Pointer to buffer to store current
     * @return true if operation succeeded
     */
    bool getCurrent(float * val);

    bool begin();
private:
    driver::Phy * m_phyDriver;
    driver::CurrentMonitor * m_currentMonitorDriver;
    driver::HighSide * m_HighSideDriver;
    /*!< Phy port number assigned from the Phy driver*/
    const uint8_t m_phy_port; 
};

} // namespace lwIOLink
