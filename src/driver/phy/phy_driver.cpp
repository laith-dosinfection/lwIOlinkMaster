#include "phy_driver.hpp"

namespace lwIOLink::driver
{

void Phy::set_transferInd_cb(PLTransferInd_cb cb)
{
    m_transferInd_cb = cb;
}

void Phy::set_fault_cb(Fault_cb cb){
    m_fault_cb = cb;
}

Phy::PhyResult Phy::setDO(bool val)
{
    return PhyResult::NotAvailable;
}

Phy::PhyResult Phy::getCurrent(const uint8_t port_no, float * val)
{   
    return PhyResult::NotAvailable;
}


} // namespace lwIOLink