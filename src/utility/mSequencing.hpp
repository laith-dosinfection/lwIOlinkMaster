#include "iolinkPort.hpp"
#include "uartPHY.hpp"

namespace IOlink
{
    uint8_t Device::GetChecksum(const uint8_t *data,
                            uint8_t length) const
    {
        uint8_t ck8 = ck8_seed;
        for (uint8_t i = 0; i < length; i++)
        {
            ck8 ^= *data++;
        }
        //Section A.1.6
        const uint8_t bit5 = ((ck8 >> 7) & 1U) ^ ((ck8 >> 5) & 1U) ^ ((ck8 >> 3) & 1U) ^ ((ck8 >> 1) & 1U);
        const uint8_t bit4 = ((ck8 >> 6) & 1U) ^ ((ck8 >> 4) & 1U) ^ ((ck8 >> 2) & 1U) ^ (ck8 & 1U);
        const uint8_t bit3 = ((ck8 >> 7) & 1U) ^ ((ck8 >> 6) & 1U);
        const uint8_t bit2 = ((ck8 >> 5) & 1U) ^ ((ck8 >> 4) & 1U);
        const uint8_t bit1 = ((ck8 >> 3) & 1U) ^ ((ck8 >> 2) & 1U);
        const uint8_t bit0 = ((ck8 >> 1) & 1U) ^ ((ck8 & 1U));
        const uint8_t ck6 =     bit5 << 5 |
                                bit4 << 4 |
                                bit3 << 3 |
                                bit2 << 2 |
                                bit1 << 1 |
                                bit0;
        return ck6;
    }
    
    class IODeviceComm
    {
    public:
    IODeviceComm(){};
    ~IODeviceComm(){};

    private:
    /// @brief M Sequence control
    /// @details M-Sequence Control octets are constructed from a packet consisting of:
    /// Bit 7: R/W bit
    /// Bit 6-5: Communication Channel
    /// Bit 0-4:  Address
    /// All the multi-octet data types shall be transmitted as a big-endian sequence, i.e. the most significant octet (MSO)
    struct m_sequence
    {
        // R/W bit
        enum r_w_t
        {
        write,
        read
        };

        // Communication Channel
        enum comm_channel_t
        {
        process,
        page,
        diagnosis,
        ISDU
        };

        // Address
        //  Other 5 bits
    };
    };
}