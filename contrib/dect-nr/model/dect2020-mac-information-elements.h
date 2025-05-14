#ifndef DECT2020_MAC_INFORMATION_ELEMENTS_H
#define DECT2020_MAC_INFORMATION_ELEMENTS_H

#include "ns3/header.h"

namespace ns3
{

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

// class Dect2020ResourceAllocationIE : public Header
// {
//   public:
//     Dect2020ResourceAllocationIE();
//     virtual ~Dect2020ResourceAllocationIE();

//     // Setter und Getter
//     void SetAllocationType(uint8_t allocationType);
//     uint8_t GetAllocationType() const;

//     void SetAdd(bool add);
//     bool GetAdd() const;

//     void SetID(bool iD);
//     bool GetID() const;

//     void SetRepeat(uint8_t repeat);
//     uint8_t GetRepeat() const;

//     void SetSystemFrameNumberFieldIncluded(bool systemFrameNumber);
//     bool GetSystemFrameNumberFieldIncluded() const;

//     void SetChannelFieldIncluded(bool channelFieldIncluded);
//     bool GetChannelFieldIncluded() const;

//     void SetRLF(bool rlf);
//     bool GetRLF() const;

//     void SetStartSubslotDownlink(uint16_t startSubslotDownlink);
//     uint16_t GetStartSubslotDownlink() const;

//     void SetStartSubslotUplink(uint16_t startSubslotUplink);
//     uint16_t GetStartSubslotUplink() const;

//     void SetLengthTypeDownlink(bool lengthTypeDownlink);
//     bool GetLengthTypeDownlink() const;

//     void SetLengthTypeUplink(bool lengthTypeUplink);
//     bool GetLengthTypeUplink() const;

//     void SetLengthDownlink(uint8_t lengthDownlink);
//     uint8_t GetLengthDownlink() const;

//     void SetLengthUplink(uint8_t lengthUplink);
//     uint8_t GetLengthUplink() const;

//     void SetShortRdId(uint16_t shortRdId);
//     uint16_t GetShortRdId() const;

//     void SetRepetition(uint8_t repetition);
//     uint8_t GetRepetition() const;

//     void SetValidity(uint8_t validity);
//     uint8_t GetValidity() const;

//     void SetSFNValue(uint8_t sfnValue);
//     uint8_t GetSFNValue() const;

//     void SetChannel(uint16_t channel);
//     uint16_t GetChannel() const;

//     void SetDectScheduledResourceFailure(uint8_t dectScheduledResourceFailure);
//     uint8_t GetDectScheduledResourceFailure() const;

//     // Überladene Methoden vom Header
//     static TypeId GetTypeId();
//     virtual TypeId GetInstanceTypeId() const override;
//     virtual uint32_t GetSerializedSize() const;
//     virtual void Serialize(Buffer::Iterator start) const;
//     virtual uint32_t Deserialize(Buffer::Iterator start);
//     virtual void Print(std::ostream& os) const;

//   private:
//     uint8_t m_allocationType;              // 2 Bit
//     bool m_add;                            // 1 Bit
//     bool m_iD;                             // 1 Bit
//     uint8_t m_repeat;                      // 3 Bit
//     bool m_systemFrameNumberFieldIncluded; // 1 Bit --> SFN
//     bool m_channelFieldIncluded;           // 1 Bit
//     bool m_rlf;                            // 1 Bit
//     uint16_t
//         m_startSubslotDownlink;    // 8 Bit. Downlink Variable. For subcarrier scaling factor <= 4,
//                                    // otherwise 9 Bit. Note: Only the 8 Bit variant is implemented
//     uint16_t m_startSubslotUplink; // 8 Bit. Uplink Variable. for subcarrier scaling factor <= 4,
//                                    // otherwise 9 Bit. Note: Only the 8 Bit variant is implemented
//     bool m_lengthTypeDownlink; // 1 Bit. Downlink Variable. 0 = length in subslots, 1 = length in
//                                // slots
//     bool m_lengthTypeUplink; // 1 Bit. Uplink Variable. 0 = length in subslots, 1 = length in slots

//     uint8_t m_lengthDownlink; // 7 Bit. Downlink Variable. The length of the resource allocation in
//                               // subslots/slots (depending on the length type field)
//     uint8_t m_lengthUplink;   // 7 Bit. Uplink Variable. The length of the resource allocation in
//                               // subslots/slots (depending on the length type field)
//     uint16_t m_shortRdId;     // 16 Bit
//     uint8_t m_repitition;     // 8 Bit
//     uint8_t m_validity;       // 8 Bit
//     uint8_t n_sfnValue;       // 8 Bit
//     uint16_t m_channel;       // 13 Bit
//     uint8_t m_dectScheduledResourceFailure; // 4 Bit
// };

} // namespace ns3

#endif