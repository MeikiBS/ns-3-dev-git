#include "dect2020-mac-common-header.h"

#include "ns3/address-utils.h"
#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020MacCommonHeader");
NS_OBJECT_ENSURE_REGISTERED(Dect2020BeaconHeader);

// *******************************************************
//            DECT2020 Beacon Header
// *******************************************************

Dect2020BeaconHeader::Dect2020BeaconHeader()
    : m_networkId(0),
      m_transmitterAddress(0)
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
Dect2020BeaconHeader::SetTransmitterAddress(uint32_t transmitterId)
{
    m_transmitterAddress = transmitterId;
}

uint32_t
Dect2020BeaconHeader::GetTransmitterAddress() const
{
    return m_transmitterAddress;
}

void
Dect2020BeaconHeader::Print(std::ostream& os) const
{
    os << "Beacon Header:" << std::endl
       << "NetworkId = 0x" << std::hex << m_networkId << std::endl
       << "TransmitterAddress = 0x" << std::hex << m_transmitterAddress << std::endl;
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

// *******************************************************
//            DECT2020 Unicast Header
// *******************************************************

Dect2020UnicastHeader::Dect2020UnicastHeader()
{
}

Dect2020UnicastHeader::~Dect2020UnicastHeader()
{
}

TypeId
Dect2020UnicastHeader::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020UnicastHeader")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020UnicastHeader>();
    return tid;
}

TypeId
Dect2020UnicastHeader::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

void
Dect2020UnicastHeader::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;
    byte0 |= (m_reset ? 1 : 0) << 4;         // Bit 3
    byte0 |= (m_sequenceNumber >> 8) & 0x0F; // Bits 4–7
    start.WriteU8(byte0);

    // Byte 1
    uint8_t byte1 = m_sequenceNumber & 0xFF; // 8 Bits
    start.WriteU8(byte1);

    // Bytes 2–5: Empfängeradresse (32 Bit)
    start.WriteHtonU32(m_receiverAddress);

    // Bytes 6–9: Senderadresse (32 Bit)
    start.WriteHtonU32(m_transmitterAddress);
}

uint32_t
Dect2020UnicastHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    // Byte 0
    uint8_t byte0 = i.ReadU8();
    m_reset = (byte0 >> 4) & 0x01;
    uint16_t sequenceHigh = byte0 & 0x0F;

    // Byte 1
    uint8_t byte1 = i.ReadU8();
    m_sequenceNumber = (sequenceHigh << 8) | byte1;

    // Bytes 2–5: Receiver Address (Big Endian)
    m_receiverAddress = i.ReadNtohU32();

    // Bytes 6–9: Transmitter Address (Big Endian)
    m_transmitterAddress = i.ReadNtohU32();

    return i.GetDistanceFrom(start); // = 10 Bytes
}

uint32_t
Dect2020UnicastHeader::GetSerializedSize() const
{
    return 10; // 2 (MAC fields) + 4 (Receiver) + 4 (Transmitter)
}

void
Dect2020UnicastHeader::Print(std::ostream& os) const
{
    os << "Dect2020UnicastHeader:" << std::endl
       << "  Reset Flag: " << std::boolalpha << m_reset << std::endl
       << "  MAC Sequence Number: " << m_sequenceNumber << std::endl
       << "  Receiver Address: 0x" << std::hex << m_receiverAddress << std::dec << std::endl
       << "  Transmitter Address: 0x" << std::hex << m_transmitterAddress << std::dec << std::endl;
}

void
Dect2020UnicastHeader::SetReset(bool reset)
{
    m_reset = reset;
}

bool
Dect2020UnicastHeader::GetReset() const
{
    return m_reset;
}

void
Dect2020UnicastHeader::SetSequenceNumber(uint16_t sequenceNumber)
{
    m_sequenceNumber = sequenceNumber & 0x0FFF;
}

uint16_t
Dect2020UnicastHeader::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

void
Dect2020UnicastHeader::SetReceiverAddress(uint32_t addr)
{
    m_receiverAddress = addr;
}

uint32_t
Dect2020UnicastHeader::GetReceiverAddress() const
{
    return m_receiverAddress;
}

void
Dect2020UnicastHeader::SetTransmitterAddress(uint32_t addr)
{
    m_transmitterAddress = addr;
}

uint32_t
Dect2020UnicastHeader::GetTransmitterAddress() const
{
    return m_transmitterAddress;
}

} // namespace ns3
