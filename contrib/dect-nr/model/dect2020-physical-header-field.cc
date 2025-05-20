#include "dect2020-physical-header-field.h"

#include "ns3/address-utils.h"
#include "ns3/log.h"

#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020PHYControlFieldType1");

NS_OBJECT_ENSURE_REGISTERED(Dect2020PHYControlFieldType1);

Dect2020PHYControlFieldType1::Dect2020PHYControlFieldType1()
{
    m_headerFormat = 0;
}

Dect2020PHYControlFieldType1::Dect2020PHYControlFieldType1(uint8_t packetLengthType,
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

Dect2020PHYControlFieldType1::~Dect2020PHYControlFieldType1()
{
}

TypeId
Dect2020PHYControlFieldType1::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020PhysicalHeaderField")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020PHYControlFieldType1>();
    return tid;
}

TypeId
Dect2020PHYControlFieldType1::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

void
Dect2020PHYControlFieldType1::Print(std::ostream& os) const
{
    os << "Physical Header Field:" << std::endl
       << "Header Format: " << static_cast<int>(m_headerFormat) << std::endl
       << "Packet length type: " << static_cast<int>(m_packetLengthType) << std::endl
       << "Packet Length: " << static_cast<int>(m_packetLength) << std::endl
       << "Short Network ID: 0x" << std::hex << static_cast<int>(m_shortNetworkID) << std::endl
       << "Transmitter Identity: 0x" << std::hex << static_cast<int>(m_transmitterIdentity)
       << std::endl
       << "Transmit power: " << static_cast<int>(m_transmitPower) << std::endl
       << "DF MCS: " << static_cast<int>(m_dFMCS) << std::endl;
}

uint32_t
Dect2020PHYControlFieldType1::GetSerializedSize(void) const
{
    return 5;
}

void
Dect2020PHYControlFieldType1::Serialize(Buffer::Iterator start) const
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
Dect2020PHYControlFieldType1::Deserialize(Buffer::Iterator start)
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
Dect2020PHYControlFieldType1::SetPacketLengthType(uint8_t packetLengthType)
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
Dect2020PHYControlFieldType1::GetPacketLengthType()
{
    return m_packetLengthType;
}

void
Dect2020PHYControlFieldType1::SetPacketLength(uint8_t packetLength)
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
Dect2020PHYControlFieldType1::GetPacketLength()
{
    return m_packetLength;
}

void
Dect2020PHYControlFieldType1::SetShortNetworkID(uint8_t shortNetworkID)
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
Dect2020PHYControlFieldType1::GetShortNetworkID()
{
    return m_shortNetworkID;
}

void
Dect2020PHYControlFieldType1::SetTransmitterIdentity(uint16_t transmitterIdentity)
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
Dect2020PHYControlFieldType1::GetTransmitterIdentity()
{
    return m_transmitterIdentity;
}

void
Dect2020PHYControlFieldType1::SetTransmitPower(uint8_t transmitPower)
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
Dect2020PHYControlFieldType1::GetTransmitPower()
{
    return m_transmitPower;
}

void
Dect2020PHYControlFieldType1::SetDFMCS(uint8_t dFMCS)
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
Dect2020PHYControlFieldType1::GetDFMCS()
{
    return m_dFMCS;
}

// *******************************************************
// DECT2020 Physical Layer Control Field: Type 2, Format 001
//            # ETSI TS 103 636-4 V2.1.1 6.2.1-2a
// *******************************************************

Dect2020PHYControlFieldType2::Dect2020PHYControlFieldType2()
{
    m_headerFormat = 1;
}

Dect2020PHYControlFieldType2::~Dect2020PHYControlFieldType2()
{
}

TypeId
Dect2020PHYControlFieldType2::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020PHYControlFieldType2")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020PHYControlFieldType2>();
    return tid;
}

TypeId
Dect2020PHYControlFieldType2::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

void
Dect2020PHYControlFieldType2::Print(std::ostream& os) const
{
    os << "PHY Control Field Type 2: "
    << " HeaderFormat=" << static_cast<uint32_t>(m_headerFormat)
       << " PacketLengthType=" << static_cast<uint32_t>(m_packetLengthType)
       << " PacketLength=" << static_cast<uint32_t>(m_packetLength)
       << " ShortNetworkID=" << static_cast<uint32_t>(m_shortNetworkID)
       << " TransmitterIdentity=" << m_transmitterIdentity
       << " TransmitPower=" << static_cast<uint32_t>(m_transmitPower)
       << " DFMCS=" << static_cast<uint32_t>(m_dFMCS) << " ReceiverIdentity=" << m_receiverIdentity
       << " NumberOfSpatialStreams=" << static_cast<uint32_t>(m_numberOfSpatialStreams)
       << " FeedbackFormat=" << static_cast<uint32_t>(m_feedbackFormat)
       << " FeedbackInfo=" << m_feedbackInfo;
}

uint32_t
Dect2020PHYControlFieldType2::GetSerializedSize(void) const
{
    return 10;
}

void
Dect2020PHYControlFieldType2::Serialize(Buffer::Iterator start) const
{
    // ETSI 103 636 04: Figure 6.2.1-2

    // Byte 0
    uint8_t byte0 = 0;
    // Bit 0-2 Header Format: Set to 001
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
    byte4 |= (m_dFMCS & 0xF);

    start.WriteU8(byte4);

    // Byte 5
    uint8_t byte5 = 0;
    byte5 |= (m_receiverIdentity >> 8) & 0xFF;

    start.WriteU8(byte5);

    // Byte 6
    uint8_t byte6 = (m_receiverIdentity) & 0xFF;

    start.WriteU8(byte6);

    // Byte 7
    uint8_t byte7 = 0;
    byte7 |= (m_numberOfSpatialStreams & 0x03) << 6;
    // Bits 2-7 Reserved

    start.WriteU8(byte7);

    // Byte 8
    uint8_t byte8 = 0;

    byte8 |= (m_feedbackFormat & 0x0F) << 4;
    byte8 |= (m_feedbackInfo >> 8) & 0X0F;

    start.WriteU8(byte8);

    // Byte 9
    uint8_t byte9 = 0;

    byte9 |= (m_feedbackInfo & 0xFF);

    start.WriteU8(byte9);
}

uint32_t
Dect2020PHYControlFieldType2::Deserialize(Buffer::Iterator start)
{
    // ETSI 103 636 04: Figure 6.2.1-2

    uint8_t byte0 = start.ReadU8();
    m_packetLengthType = (byte0 >> 4) & 0x01;
    m_packetLength = byte0 & 0x0F;

    uint8_t byte1 = start.ReadU8();
    m_shortNetworkID = byte1;

    uint8_t byte2 = start.ReadU8();
    uint8_t byte3 = start.ReadU8();
    m_transmitterIdentity = (byte2 << 8) | byte3;

    uint8_t byte4 = start.ReadU8();
    m_transmitPower = (byte4 >> 4) & 0x0F;
    m_dFMCS = byte4 & 0x0F;

    uint8_t byte5 = start.ReadU8();
    uint8_t byte6 = start.ReadU8();
    m_receiverIdentity = (byte5 << 8) | byte6;

    uint8_t byte7 = start.ReadU8();
    m_numberOfSpatialStreams = (byte7 >> 6) & 0x03;

    uint8_t byte8 = start.ReadU8();
    m_feedbackFormat = (byte8 >> 4) & 0x0F;
    m_feedbackInfo = (byte8 & 0x0F) << 8;

    uint8_t byte9 = start.ReadU8();
    m_feedbackInfo |= byte9;

    return GetSerializedSize();
}

void
Dect2020PHYControlFieldType2::SetPacketLengthType(uint8_t packetLengthType)
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
Dect2020PHYControlFieldType2::GetPacketLengthType()
{
    return m_packetLengthType;
}

void
Dect2020PHYControlFieldType2::SetPacketLength(uint8_t packetLength)
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
Dect2020PHYControlFieldType2::GetPacketLength()
{
    return m_packetLength;
}

void
Dect2020PHYControlFieldType2::SetShortNetworkID(uint8_t shortNetworkID)
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
Dect2020PHYControlFieldType2::GetShortNetworkID()
{
    return m_shortNetworkID;
}

void
Dect2020PHYControlFieldType2::SetTransmitterIdentity(uint16_t transmitterIdentity)
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
Dect2020PHYControlFieldType2::GetTransmitterIdentity()
{
    return m_transmitterIdentity;
}

void
Dect2020PHYControlFieldType2::SetTransmitPower(uint8_t transmitPower)
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
Dect2020PHYControlFieldType2::GetTransmitPower()
{
    return m_transmitPower;
}

void
Dect2020PHYControlFieldType2::SetDFMCS(uint8_t dFMCS)
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
Dect2020PHYControlFieldType2::GetDFMCS()
{
    return m_dFMCS;
}

void
Dect2020PHYControlFieldType2::SetReceiverIdentity(uint16_t receiverIdentity)
{
    m_receiverIdentity = receiverIdentity;
}

uint16_t
Dect2020PHYControlFieldType2::GetReceiverIdentity() const
{
    return m_receiverIdentity;
}

void
Dect2020PHYControlFieldType2::SetNumberOfSpatialStreams(uint8_t numberOfSpatialStreams)
{
    m_numberOfSpatialStreams = numberOfSpatialStreams & 0x03;
}

uint8_t
Dect2020PHYControlFieldType2::GetNumberOfSpatialStreams() const
{
    return m_numberOfSpatialStreams;
}

void
Dect2020PHYControlFieldType2::SetFeedbackFormat(uint8_t feedbackFormat)
{
    m_feedbackFormat = feedbackFormat & 0x0F;
}

uint8_t
Dect2020PHYControlFieldType2::GetFeedbackFormat() const
{
    return m_feedbackFormat;
}

void
Dect2020PHYControlFieldType2::SetFeedbackInfo(uint16_t feedbackInfo)
{
    m_feedbackInfo = feedbackInfo & 0x0FFF;
}

uint16_t
Dect2020PHYControlFieldType2::GetFeedbackInfo() const
{
    return m_feedbackInfo;
}


} // namespace ns3
