#ifndef DECT2020_PHYSICAL_HEADER_FIELD
#define DECT2020_PHYSICAL_HEADER_FIELD

#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/address.h"

namespace ns3
{

class Dect2020PhysicalHeaderField : public Header
{
  public:
    Dect2020PhysicalHeaderField();
    virtual ~Dect2020PhysicalHeaderField();


    // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

//   private:
    uint8_t m_headerFormat;
    uint8_t m_packetLengthType;
    uint8_t m_packetLength;
    uint8_t m_shortNetworkID;
    uint16_t m_transmitterIdentity;
    uint8_t m_transmitPower;
    uint8_t m_dFMCS;
    uint32_t m_networkId;          // 24 Bits relevant
    uint32_t m_transmitterAddress; // 32 Bits
};

} // namespace ns3

#endif // DECT2020_PHYSICAL_HEADER_FIELD
