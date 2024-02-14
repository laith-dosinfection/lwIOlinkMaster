#include <Arduino.h>
#include <lwIOLinkMaster.hpp>

lwIOLink::driver::phy::TIOL112::tiol_cfg driver_cfg1;

lwIOLink::driver::phy::TIOL112 Driver1(&driver_cfg1);
constexpr uint8_t total_ports = 1;

lwIOLink::Port Port1 = lwIOLink::Port(&Driver1, 1);

lwIOLink::Port * PortList[total_ports] = {
    &Port1
};

lwIOLink::Master master(PortList, total_ports);

void setup(void)
{
  const bool init = master.begin();
}

void loop(){
}
