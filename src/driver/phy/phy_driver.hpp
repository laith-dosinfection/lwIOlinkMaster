#pragma once
#include <cstdint>

namespace lwIOLink::driver
{

class Phy{
    public:
    // Max buffer size for IOLink device is 32 bytes PD + 32 bytes OD + checksum
    static constexpr uint8_t RX_BUFF_SIZE{65};
    // Enumeration for PHY driver result
    enum class PhyResult {
        Success,
        HwFail,
        IncorrectMode,
        NotAvailable // cannot do operation in current phy mode
    };

    enum class fault_e {
        Overcurrent,
        UnderVoltage,
        OverTemp,
        MSeqTimeout
    };

    enum class COM_e {
        COM1=4800,
        COM2=38400,
        COM3=230400,
        SIO=0
    };

    struct Hw_cfg_t{
    };
    /**
     * @brief Callback for fault in the ports
     * @param port_no Phy port number where the fault occured
     * @param fault Fault type
     */
    using Fault_cb = void (*)(uint8_t port_no, fault_e fault);

    /**
     * @brief IOlink PL Transfer indication service callback
     * @param port_no Port number
     * @param pBuf Pointer to MSeq returned by the IO-Link device
     * @param len Length of the MSEQ returedn by the IO-Link device
     */
    using PLTransferInd_cb = void (*)(uint8_t port_no, const uint8_t * pBuf, const uint8_t len);
    protected:
    /**
     * @brief Construct a new Phy object
     * 
     * @param port_count Number of ports that the PHY can handle
     */
    Phy(const uint8_t port_count):
        m_totalPorts(port_count){
            
        };
   public:
        /**
         * @brief Driver initialization
         * 
         */
        virtual bool begin() =0;
        /**
         * @brief Set the transferInd cb object
         * @details The callback set over this method will be 
         *           used by the specific phy driver implementation
         *           to indicate the port that a new MSEQ from 
         *          the IO-Link device has been recieved
         * @param cb 
         */
        void set_transferInd_cb(PLTransferInd_cb cb);

        /**
         * @brief Set the fault cb for the phy driver
         * @details Callback which should be used by the
         *          phy driver implementation to indicate a fault
         * @param cb 
         */
        void set_fault_cb(Fault_cb cb);

        /**
         * @brief Set the Port as a digital output
         * @details Available when Phy port is set as SIO mode
         * 
         * @param val 
         * @return PhyResult 
         */
        virtual PhyResult setDO(bool val);
        /**
         * @brief Get the current consumption of a specific phy port
         * @details If the PHY allows to read current consumption
         *           the driver should implement this function
         * @param port_no Port number
         * @param val Pointer where to write current value
         * @return PhyResult 
         */
        virtual PhyResult getCurrent(const uint8_t port_no, float * val);

        /**
         * @brief Wake up generation
         * @details Driver specific implementation of the wakeup generation
         * @param port_no 
         * @return true 
         * @return false 
         */
        virtual bool wakeup(const uint8_t port_no)=0;
        /**
         * @brief Transfer request of SDCI data, could be over UART, I2C,etc
         * 
         * @param port_no Port number for Phy
         * @param pBuf Buffer to transmit
         * @param len Transmission length
         * @param rx_len Expected RX length
         * @return true if succeeds
         */
        virtual bool PLTransferReq(const uint8_t port_no, const uint8_t * pBuf, const uint8_t len,const uint8_t rx_len)=0;


        /**
         * @brief Function that is called in a loop to poll
         *        for new data from IO-Link device
         * @details Since Arduino does not have an API to get UART data
         *          over interrupt call this in a loop or in a state machine 
         *          that is looped over the lwIOLink::Port class
         * 
         */
        virtual void RecieveTask() = 0;


        /**
         * @brief Set the COM Mode for the Phy
         * 
         * @param mode 
         */
        virtual void setCOM_Mode(const uint8_t port_no, const COM_e mode)=0;


   protected:
      Fault_cb m_fault_cb;
      PLTransferInd_cb m_transferInd_cb;
      const uint8_t m_totalPorts;
};

} // namespace lwIOLink
