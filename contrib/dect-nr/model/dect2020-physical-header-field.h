#ifndef DECT2020_PHYSICAL_HEADER_FIELD
#define DECT2020_PHYSICAL_HEADER_FIELD

#include "ns3/address.h"
#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3
{

// TODO: Implement Physical Layer Control Field Type 1

class Dect2020PhysicalHeaderField : public Header
{
  public:
    Dect2020PhysicalHeaderField();
    Dect2020PhysicalHeaderField(uint8_t packetLengthType,
                                uint8_t packetLength,
                                uint8_t shortNetworkID,
                                uint16_t transmitterIdentity,
                                uint8_t transmitPower,
                                uint8_t dFMCS);
    virtual ~Dect2020PhysicalHeaderField();

    // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

    // Setter und Getter
    void SetPacketLengthType(uint8_t packetLengthType);
    uint8_t GetPacketLengthType();
    void SetPacketLength(uint8_t packetLength);
    uint8_t GetPacketLength();
    void SetShortNetworkID(uint8_t shortNetworkID);
    uint8_t GetShortNetworkID();
    void SetTransmitterIdentity(uint16_t transmitterIdentity);
    uint16_t GetTransmitterIdentity();
    void SetTransmitPower(uint8_t transmitPower);
    uint8_t GetTransmitPower();
    void SetDFMCS(uint8_t dFMCS);
    uint8_t GetDFMCS();

  private:
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
