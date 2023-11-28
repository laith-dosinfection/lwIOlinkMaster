#pragma once
#include "../phy_driver.hpp"
#include <Arduino.h>

namespace lwIOLink::driver::phy
{

class TIOL112:public Phy{
    public:
    
    struct tiol_cfg : public Phy::Hw_cfg_t {
        uint8_t tx_en;
        uint8_t fault_pin;
        Stream * serial;
    };

    static constexpr uint8_t total_ports = 1;

    TIOL112(const tiol_cfg * cfg):
        Phy(total_ports),
        m_hwCfg(cfg){}

    bool begin() override;
    bool wakeup(const uint8_t port_no) override;
    bool PLTransferReq(const uint8_t port_no,
                        const uint8_t * pBuf,
                        const uint8_t len,
                        const uint8_t rx_len) override;
    void RecieveTask() override;
    void setCOM_Mode(const uint8_t port_no,const COM_e mode) override;
    private:
        const tiol_cfg * m_hwCfg;
        void fault_interrupt();
        bool m_rxEnable = false;
        uint8_t m_rxCnt = 0;
        uint8_t m_rxBuf[RX_BUFF_SIZE]{0};
        uint32_t m_rxTimeStart = 0;
        uint32_t m_rxTimeout;
        COM_e m_com_mode;
        uint8_t m_expRXLen{0xFF};

};

} //lwIOLink::driver::phy