#include "tiol112.hpp"
#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP32
#include "driver/uart.h"
#include "soc/uart_reg.h"
#endif	//ARDUINO_ARCH_ESP32

namespace lwIOLink::driver::phy
{


// TODO This should be run in a loop to detect falling edge of fault pin
// unfortunatley Arduino API attach interrupt does not 
// provide an argument to set context for interrupt
void TIOL112::fault_interrupt()
{
    if(m_fault_cb) {
        // always one since TIOL112 has only one port
        constexpr uint8_t port_no_fault = 1; 
        // TIOL112 does not differentiate between 
        // faults so send all three possible faults
        m_fault_cb(port_no_fault, fault_e::Overcurrent);
        m_fault_cb(port_no_fault, fault_e::UnderVoltage);
        m_fault_cb(port_no_fault, fault_e::OverTemp);
    }
}

bool TIOL112::begin()
{
    pinMode(m_hwCfg->tx_en, OUTPUT);
    pinMode(m_hwCfg->fault_pin, INPUT);
    setCOM_Mode(1, COM_e::SIO);
    return true;
}

bool TIOL112::wakeup(const uint8_t port_no)
{
    return true;
}

bool TIOL112::PLTransferReq(const uint8_t port_no,
                    const uint8_t * pBuf,
                    const uint8_t len,
                    const uint8_t rx_len)
{
    if (m_com_mode == COM_e::SIO) {
        return false;
    }
    m_expRXLen = rx_len;
    digitalWrite(m_hwCfg->tx_en, HIGH);
    for (uint8_t i = 0; i < len; i++)
    {
        m_hwCfg->serial->write(*(pBuf + i));
    }
    m_hwCfg->serial->flush();	//Wait until transmission is finished
    digitalWrite(m_hwCfg->tx_en, LOW);

    constexpr uint8_t frame_bits = 10; // stop,8 data+ parity 
    uint32_t tbit =0;

    m_rxCnt = 0;
    m_rxTimeStart = micros();
    if (m_com_mode == COM_e::COM1) {
        tbit = 209; // 208,3 us 
    } else if(m_com_mode == COM_e::COM2) {
        tbit= 261; // 260,41 us
    } else {
        tbit = 5; // 4,34 us
    }
    const uint32_t uart_frame_duration  =  (tbit*frame_bits); 
    const uint32_t tA = uart_frame_duration; // figure a.15
    const uint32_t t2 = tbit; // figure a.15
    m_rxTimeout =  (uart_frame_duration * rx_len) + tA + t2*(rx_len-1) ;
    memset(&m_rxBuf[0],0, RX_BUFF_SIZE);
    m_rxEnable = true;
    return true;
}

void TIOL112::setCOM_Mode(const uint8_t port_no,const COM_e mode)
{
    m_com_mode = mode;
    if (mode != COM_e::SIO) {
        static_cast<HardwareSerial*>(m_hwCfg->serial)->begin(
            static_cast<uint32_t> (COM_e::COM3),
            SERIAL_8E1);
#if defined(ARDUINO_ARCH_ESP32)
        uint8_t uart_num;
        if (m_hwCfg->serial == &Serial)
        {
            uart_num = 0;
        }
        else if (m_hwCfg->serial == &Serial1)
        {
            uart_num = 1;
        }
        else
        {
            uart_num = 2;
        }
        uart_intr_config_t uart_intr;
        uart_intr.intr_enable_mask =    UART_RXFIFO_FULL_INT_ENA_M |
                                        UART_RXFIFO_TOUT_INT_ENA_M |
                                        UART_FRM_ERR_INT_ENA_M |
                                        UART_RXFIFO_OVF_INT_ENA_M |
                                        UART_BRK_DET_INT_ENA_M |
                                        UART_PARITY_ERR_INT_ENA_M;
        uart_intr.rxfifo_full_thresh = 1;
        uart_intr.rx_timeout_thresh = 10;
        uart_intr.txfifo_empty_intr_thresh = 10;
        uart_intr_config(uart_num, &uart_intr);
#endif
    }
}

void TIOL112::RecieveTask()
{   
    // always one since TIOL112 has only one port
    constexpr uint8_t port_no = 1; 
    if (m_rxEnable) {
        while (m_hwCfg->serial->available() > 0
                && m_rxCnt < RX_BUFF_SIZE
                && m_rxCnt < m_expRXLen
                && m_rxTimeout < micros() - m_rxTimeStart)
        {
            m_rxBuf[m_rxCnt++] = m_hwCfg->serial->read();
        }
        if (m_rxTimeout > micros() -m_rxTimeStart) {
            if (m_fault_cb) {
                m_rxEnable = false;
                m_fault_cb(port_no, fault_e::MSeqTimeout);
            }
        }
        if (m_rxCnt == m_expRXLen) {
            m_rxEnable = false;
            if (m_transferInd_cb) {
                m_transferInd_cb(port_no, m_rxBuf, m_rxCnt);
            }
        }
    }
}


} //lwIOLink::driver::phy