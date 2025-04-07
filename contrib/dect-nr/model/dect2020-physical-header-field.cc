#include "dect2020-physical-header-field.h"

#include "ns3/address-utils.h"
#include "ns3/log.h"

#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020PhysicalHeaderField");

NS_OBJECT_ENSURE_REGISTERED(Dect2020PhysicalHeaderField);

Dect2020PhysicalHeaderField::Dect2020PhysicalHeaderField()
{
    m_headerFormat = 0;
}

Dect2020PhysicalHeaderField::Dect2020PhysicalHeaderField(uint8_t packetLengthType,
                                                         uint8_t packetLength,
                                                         uint8_t shortNetworkID,
                                                         uint16_t transmitterIdentity,
                                                         uint8_t transmitPower,
                                                         uint8_t dFMCS)
{
    SetPacketLengthType(packetLengthType);
    SetPacketLength(packetLength);
    SetShortNetworkID(shortNetworkID);
    SetTransmitterIdentity(transmitterIdentity);
    SetTransmitPower(transmitPower);
    SetDFMCS(dFMCS);
}

Dect2020PhysicalHeaderField::~Dect2020PhysicalHeaderField()
{
}

TypeId
Dect2020PhysicalHeaderField::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020PhysicalHeaderField")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020PhysicalHeaderField>();
    return tid;
}

TypeId
Dect2020PhysicalHeaderField::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

void
Dect2020PhysicalHeaderField::Print(std::ostream& os) const
{
    os << "Header Format: " << static_cast<int>(m_headerFormat) << std::endl
       << "Packet length type: " << static_cast<int>(m_packetLengthType) << std::endl
       << "Packet Length: " << static_cast<int>(m_packetLength) << std::endl
       << "Short Network ID: 0x" << std::hex << static_cast<int>(m_shortNetworkID) << std::endl
       << "Transmitter Identity: 0x" << std::hex << static_cast<int>(m_transmitterIdentity)
       << std::endl
       << "Transmit power: " << static_cast<int>(m_transmitPower) << std::endl
       << "DF MCS: " << static_cast<int>(m_dFMCS) << std::endl;
}

uint32_t
Dect2020PhysicalHeaderField::GetSerializedSize(void) const
{
    return 5;
}

void
Dect2020PhysicalHeaderField::Serialize(Buffer::Iterator start) const
{
    // ETSI 103 636 04: Figure 6.2.1-1

    // Byte 0
    uint8_t byte0 = 0;
    // Bit 0-2 Header Format: Set to 000
    byte0 |= (m_packetLengthType << 4);
    byte0 |= (m_packetLength & 0x0F);

    start.WriteU8(byte0);

    // Byte 1
    uint8_t byte1 = (m_shortNetworkID & 0xFF);

    start.WriteU8(byte1);

    // Byte 2
    uint8_t byte2 = (m_transmitterIdentity >> 8) & 0xFF;

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = (m_transmitterIdentity) & 0xFF;

    start.WriteU8(byte3);

    // Byte 4
    uint8_t byte4 = (m_transmitPower & 0x0F) << 4;
    // Bit 4 reserved
    byte4 |= (m_dFMCS & 0x3);

    start.WriteU8(byte4);
}

uint32_t
Dect2020PhysicalHeaderField::Deserialize(Buffer::Iterator start)
{
    uint32_t bytesRead = 0;
    uint8_t byte0 = start.ReadU8();
    bytesRead++;

    // Bit 0-2 set to 0 (Header format)
    m_packetLengthType = (byte0 & (1 << 4)) != 0;
    m_packetLength = (byte0 & 0x0F);

    // Byte 1
    uint8_t byte1 = start.ReadU8();
    bytesRead++;
    m_shortNetworkID = (byte1 & 0xFF);

    // Byte 2-3
    uint8_t byte2 = start.ReadU8();
    uint8_t byte3 = start.ReadU8();

    m_transmitterIdentity = (byte2 << 8) | byte3;
    bytesRead += 2;

    // Byte 4
    uint8_t byte4 = start.ReadU8();
    bytesRead++;
    m_transmitPower = (byte4 & 0xF0) >> 4;
    m_dFMCS = (byte4 & 0x03);

    return bytesRead;
}

void
Dect2020PhysicalHeaderField::SetPacketLengthType(uint8_t packetLengthType)
{
    if (packetLengthType == 0 || packetLengthType == 1)
    {
        m_packetLengthType = packetLengthType;
    }
    else
    {
        NS_LOG_WARN("Invalid Packet Length Type. Must be 0 or 1.");
    }
}

uint8_t
Dect2020PhysicalHeaderField::GetPacketLengthType()
{
    return m_packetLengthType;
}

void
Dect2020PhysicalHeaderField::SetPacketLength(uint8_t packetLength)
{
    if (packetLength >= 0 && packetLength <= 15)
    {
        m_packetLength = packetLength;
    }
    else
    {
        NS_LOG_WARN("Invalid Packet Length. Must be between 0 and 15");
    }
}

uint8_t
Dect2020PhysicalHeaderField::GetPacketLength()
{
    return m_packetLength;
}

void
Dect2020PhysicalHeaderField::SetShortNetworkID(uint8_t shortNetworkID)
{
    if (shortNetworkID >= 0 && shortNetworkID <= pow(2, 8) - 1)
    {
        m_shortNetworkID = shortNetworkID;
    }
    else
    {
        NS_LOG_WARN("Invalid Short Network ID. Must be between 0 and " << (pow(2, 8) - 1));
    }
}

uint8_t
Dect2020PhysicalHeaderField::GetShortNetworkID()
{
    return m_shortNetworkID;
}

void
Dect2020PhysicalHeaderField::SetTransmitterIdentity(uint16_t transmitterIdentity)
{
    if (transmitterIdentity >= 0 && transmitterIdentity <= pow(2, 16) - 1)
    {
        m_transmitterIdentity = transmitterIdentity;
    }
    else
    {
        NS_LOG_WARN("Invalid Transmitter Identity. Must be between 0 and " << (pow(2, 16) - 1));
    }
}

uint16_t
Dect2020PhysicalHeaderField::GetTransmitterIdentity()
{
    return m_transmitterIdentity;
}

void
Dect2020PhysicalHeaderField::SetTransmitPower(uint8_t transmitPower)
{
    if (transmitPower >= 0 && transmitPower <= (pow(2, 4) - 1))
    {
        m_transmitPower = transmitPower;
    }
    else
    {
        NS_LOG_WARN("Invalid Transmit Power. Must be between 0 and " << (pow(2, 4) - 1));
    }
}

uint8_t
Dect2020PhysicalHeaderField::GetTransmitPower()
{
    return m_transmitPower;
}

void
Dect2020PhysicalHeaderField::SetDFMCS(uint8_t dFMCS)
{
    if (dFMCS >= 0 && dFMCS <= pow(2, 3) - 1)
    {
        m_dFMCS = dFMCS;
    }
    else
    {
        NS_LOG_WARN("Invalid DF MCS. Must be between 0 and " << pow(2, 3) - 1);
    }
}

uint8_t
Dect2020PhysicalHeaderField::GetDFMCS()
{
    return m_dFMCS;
}

} // namespace ns3
