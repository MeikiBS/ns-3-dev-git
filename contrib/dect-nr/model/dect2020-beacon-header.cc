#include "dect2020-beacon-header.h"

#include "ns3/address-utils.h"
#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020BeaconHeader");

NS_OBJECT_ENSURE_REGISTERED(Dect2020BeaconHeader);

Dect2020BeaconHeader::Dect2020BeaconHeader()
    : m_networkId(0),
      m_transmitterAddress(Address())
{
}

Dect2020BeaconHeader::~Dect2020BeaconHeader()
{
}

TypeId
Dect2020BeaconHeader::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020BeaconHeader")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020BeaconHeader>();
    return tid;
}

TypeId
Dect2020BeaconHeader::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

void
Dect2020BeaconHeader::SetNetworkId(uint32_t networkId)
{
    m_networkId = networkId & 0xFFFFFF; // Nur die unteren 24 Bits speichern
}

uint32_t
Dect2020BeaconHeader::GetNetworkId() const
{
    return m_networkId;
}

void
Dect2020BeaconHeader::SetTransmitterAddress(Address transmitterId)
{
    m_transmitterAddress = transmitterId;
}

Address
Dect2020BeaconHeader::GetTransmitterAddress() const
{
    return m_transmitterAddress;
}

void
Dect2020BeaconHeader::Print(std::ostream& os) const
{
    os << "NetworkId=0x" << std::hex << m_networkId << ", TransmitterAddress=0x" << std::hex
       << m_transmitterAddress;
}

uint32_t
Dect2020BeaconHeader::GetSerializedSize(void) const
{
    return 7; // 24 Bits Network ID + 32 Bits Transmitter Address = 56 Bits => 7 Bytes
}

void
Dect2020BeaconHeader::Serialize(Buffer::Iterator start) const
{
    // Network ID (24 Bits)
    start.WriteU8((m_networkId >> 16) & 0xFF);
    start.WriteU8((m_networkId >> 8) & 0xFF);
    start.WriteU8(m_networkId & 0xFF);

    // Transmitter Address (32 Bits)
    start.WriteHtonU32(m_transmitterAddress);
}

uint32_t
Dect2020BeaconHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    // Network ID (24 Bits)
    uint32_t networkId = 0;
    networkId |= (i.ReadU8() << 16);
    networkId |= (i.ReadU8() << 8);
    networkId |= i.ReadU8();
    m_networkId = networkId;

    // Transmitter Address (32 Bits)
    m_transmitterAddress = i.ReadNtohU32();

    return GetSerializedSize();
}

} // namespace ns3
