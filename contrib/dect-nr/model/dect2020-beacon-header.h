#ifndef DECT2020_BEACON_HEADER_H
#define DECT2020_BEACON_HEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/address.h"

namespace ns3
{

class Dect2020BeaconHeader : public Header
{
  public:
    Dect2020BeaconHeader();
    virtual ~Dect2020BeaconHeader();

    // Setter und Getter
    void SetNetworkId(uint32_t networkId);
    uint32_t GetNetworkId() const;

    void SetTransmitterAddress(uint32_t transmitterId);
    uint32_t GetTransmitterAddress() const;

    // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

  private:
    uint32_t m_networkId;          // 24 Bits relevant
    uint32_t m_transmitterAddress; // 32 Bits
};

} // namespace ns3

#endif // DECT2020_BEACON_HEADER_H
