#ifndef _mSequences_HPP_
#define _mSequences_HPP_

#ifndef _sdci_HPP_
#include "sdci.hpp"
#endif //SDCI

namespace IOlink
{
    class sdciMessages
    {

        public:

                /// @brief M Sequence control
        /// @details M-Sequence Control octets are constructed from a packet consisting of:
        /// Bit 7: R/W bit
        /// Bit 6-5: Communication Channel
        /// Bit 0-4:  Address
        /// All the multi-octet data types shall be transmitted as a big-endian sequence, i.e. the most significant octet (MSO)

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

        enum m_sequence_type{
            TYPE0,
            TYPE1,
            TYPE2,
            RESERVED
        };
        
        enum PDstatus{
            VALID,
            INVALID
        };

        enum EventFlag{
            NOEVENT,
            EVENT
        };

        /// TODO: the rest need to be added Table B.1
        enum Address : uint8_t{
            MasterCommand = 0x00,
            MasterCycleTime = 0x01,
            MinCycleTime = 0x02,
            MSequenceCapability = 0x03,
            RevisionID = 0x04
        };
        
        uint8_t ck8_seed = 0x52;

        uint8_t CKS(EventFlag e, PDstatus s, uint8_t _checkSum){
            uint8_t checkSum = _checkSum;
            uint8_t event = (uint8_t)e << 7;
            uint8_t status = (uint8_t)s << 6;
            return event || status || checkSum;
        };

        bool CKS(uint8_t _data){
            uint8_t data = _data;
            bool event = (data & 0b10000000) != 0;
            bool status = (data & 0b01000000) != 0;
            uint8_t checkSum = (data & 0b0011111);
        };

        uint8_t CKT(m_sequence_type _type, uint8_t _checkSum){
            uint8_t checkSum = _checkSum;
            uint8_t type = (uint8_t)_type << 6;
            return type || checkSum;
        };

        uint8_t MC(r_w_t _rw, comm_channel_t _c, uint8_t _data){
            uint8_t data = _data;
            uint8_t rw = (uint8_t)_rw << 7;
            uint8_t c = (uint8_t)_c << 6;
            return rw || c || data;
        }


        uint8_t GetChecksum(const uint8_t *data,
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
        };
    };
}

#endif //mSequences