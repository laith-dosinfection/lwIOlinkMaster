#include "iolinkPort.hpp"
#include "uartPHY.hpp"

namespace IOlink
{
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