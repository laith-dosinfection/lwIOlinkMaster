#include "port.hpp"

namespace lwIOLink {

bool Port::begin() {
    auto const phy_res = m_phyDriver->begin();
    return phy_res;
}

// Method to set digital output
bool Port::setDO(bool val) {
    if (m_HighSideDriver) {
        return m_HighSideDriver->setDO(val);
    } else {
        const auto res = m_phyDriver->setDO(val);
        return res == driver::Phy::PhyResult::Success;
    }
}

bool Port::getCurrent(float * val) {
    if (m_currentMonitorDriver) {
        return m_currentMonitorDriver->getCurrent(val);
    } else {
        const auto res = m_phyDriver->getCurrent(m_phy_port, val);
        return res == driver::Phy::PhyResult::Success;
    }
}

}