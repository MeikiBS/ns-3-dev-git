#include "dect2020-mac-information-elements.h"

#include "ns3/log.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("Dect2020MACInformationElements");


// *******************************************************
//            DECT2020 Resource Allocation IE
// *******************************************************

// ----------------------------------------------------------------------------------
// [Developer Note]
// This section was initially intended to implement the Resource Allocation IE
// as specified in section 6.4.3.3 (Dect2020ResourceAllocationIE).
//
// During the implementation process, it became clear that this was not the correct
// Information Element required in this context. Instead, the Random Access Resource IE
// (section 6.4.3.2) should be used.
//
// The current structure, fields, and methods in this class relate to the wrong IE
// and have therefore not been completed.
//
// Please restructure or remove this code as needed once the correct IE is implemented.
// Next step would be the Serialize Method.
// ----------------------------------------------------------------------------------
//
// [Meikel Kersting, May 2025]

// Dect2020ResourceAllocationIE::Dect2020ResourceAllocationIE()
// {
// }

// Dect2020ResourceAllocationIE::~Dect2020ResourceAllocationIE()
// {
// }

// TypeId
// Dect2020ResourceAllocationIE::GetTypeId(void)
// {
//     static TypeId tid = TypeId("ns3::Dect2020ResourceAllocationIE")
//                             .SetParent<Header>()
//                             .SetGroupName("Dect2020")
//                             .AddConstructor<Dect2020ResourceAllocationIE>();
//     return tid;
// }

// TypeId
// Dect2020ResourceAllocationIE::GetInstanceTypeId(void) const
// {
//     return GetTypeId();
// }

// void
// Dect2020ResourceAllocationIE::Serialize(Buffer::Iterator start) const
// {
//     // Byte 0
//     uint8_t byte0 = 0;
//     byte0 |= (m_allocationType & 0x03) << 6;             // Bits 0-1
//     byte0 |= (m_add ? 1 : 0) << 5;                       // Bit 2
//     byte0 |= (m_iD ? 1 : 0) << 4;                        // Bit 3
//     byte0 |= (m_repeat & 0x07) << 1;                     // Bits 4-6
//     byte0 |= (m_systemFrameNumberFieldIncluded ? 1 : 0); // Bit 7
//     start.WriteU8(byte0);

//     // when the Allocation Type is set to 00 - release all scheduled resources
//     if ((m_allocationType & 0x03) == 0b00)
//     {
//         return;
//     }

//     // Byte 1
//     uint8_t byte1 = 0;
//     byte1 |= (m_channelFieldIncluded ? 1 : 0) << 7;
//     byte1 |= (m_rlf ? 1 : 0) << 6;
//     // Bit 2-7 reserved
//     start.WriteU8(byte1);

//     if(m_allocationType == 1 || m_allocationType == 3)
//     {

//     }

// }

// void
// Dect2020ResourceAllocationIE::SetAllocationType(uint8_t allocationType)
// {
//     m_allocationType = allocationType & 0x03; // 2 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetAllocationType() const
// {
//     return m_allocationType;
// }

// void
// Dect2020ResourceAllocationIE::SetAdd(bool add)
// {
//     m_add = add;
// }

// bool
// Dect2020ResourceAllocationIE::GetAdd() const
// {
//     return m_add;
// }

// void
// Dect2020ResourceAllocationIE::SetID(bool iD)
// {
//     m_iD = iD;
// }

// bool
// Dect2020ResourceAllocationIE::GetID() const
// {
//     return m_iD;
// }

// void
// Dect2020ResourceAllocationIE::SetRepeat(uint8_t repeat)
// {
//     m_repeat = repeat & 0x07; // 3 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetRepeat() const
// {
//     return m_repeat;
// }

// void
// Dect2020ResourceAllocationIE::SetSystemFrameNumberFieldIncluded(bool systemFrameNumber)
// {
//     m_systemFrameNumberFieldIncluded = systemFrameNumber;
// }

// bool
// Dect2020ResourceAllocationIE::GetSystemFrameNumberFieldIncluded() const
// {
//     return m_systemFrameNumberFieldIncluded;
// }

// void
// Dect2020ResourceAllocationIE::SetChannelFieldIncluded(bool channelFieldIncluded)
// {
//     m_channelFieldIncluded = channelFieldIncluded;
// }

// bool
// Dect2020ResourceAllocationIE::GetChannelFieldIncluded() const
// {
//     return m_channelFieldIncluded;
// }

// void
// Dect2020ResourceAllocationIE::SetRLF(bool rlf)
// {
//     m_rlf = rlf;
// }

// bool
// Dect2020ResourceAllocationIE::GetRLF() const
// {
//     return m_rlf;
// }

// void
// Dect2020ResourceAllocationIE::SetStartSubslotDownlink(uint16_t startSubslotDownlink)
// {
//     m_startSubslotDownlink = startSubslotDownlink & 0xFF; // 8 Bit
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetStartSubslotDownlink() const
// {
//     return m_startSubslotDownlink;
// }

// void
// Dect2020ResourceAllocationIE::SetStartSubslotUplink(uint16_t startSubslotUplink)
// {
//     m_startSubslotUplink = startSubslotUplink & 0xFF; // 8 Bit
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetStartSubslotUplink() const
// {
//     return m_startSubslotUplink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthTypeDownlink(bool lengthTypeDownlink)
// {
//     m_lengthTypeDownlink = lengthTypeDownlink;
// }

// bool
// Dect2020ResourceAllocationIE::GetLengthTypeDownlink() const
// {
//     return m_lengthTypeDownlink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthTypeUplink(bool lengthTypeUplink)
// {
//     m_lengthTypeUplink = lengthTypeUplink;
// }

// bool
// Dect2020ResourceAllocationIE::GetLengthTypeUplink() const
// {
//     return m_lengthTypeUplink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthDownlink(uint8_t lengthDownlink)
// {
//     m_lengthDownlink = lengthDownlink & 0x7F; // 7 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetLengthDownlink() const
// {
//     return m_lengthDownlink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthUplink(uint8_t lengthUplink)
// {
//     m_lengthUplink = lengthUplink & 0x7F; // 7 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetLengthUplink() const
// {
//     return m_lengthUplink;
// }

// void
// Dect2020ResourceAllocationIE::SetShortRdId(uint16_t shortRdId)
// {
//     m_shortRdId = shortRdId;
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetShortRdId() const
// {
//     return m_shortRdId;
// }

// void
// Dect2020ResourceAllocationIE::SetRepetition(uint8_t repetition)
// {
//     m_repitition = repetition;
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetRepetition() const
// {
//     return m_repitition;
// }

// void
// Dect2020ResourceAllocationIE::SetValidity(uint8_t validity)
// {
//     m_validity = validity;
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetValidity() const
// {
//     return m_validity;
// }

// void
// Dect2020ResourceAllocationIE::SetSFNValue(uint8_t sfnValue)
// {
//     n_sfnValue = sfnValue;
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetSFNValue() const
// {
//     return n_sfnValue;
// }

// void
// Dect2020ResourceAllocationIE::SetChannel(uint16_t channel)
// {
//     m_channel = channel & 0x1FFF; // 13 Bit
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetChannel() const
// {
//     return m_channel;
// }

// void
// Dect2020ResourceAllocationIE::SetDectScheduledResourceFailure(uint8_t failure)
// {
//     m_dectScheduledResourceFailure = failure & 0x0F; // 4 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetDectScheduledResourceFailure() const
// {
//     return m_dectScheduledResourceFailure;
// }

} // namespace ns3