#include "dect2020-mac-multiplexing-header.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020MacMultiplexingHeader");
NS_OBJECT_ENSURE_REGISTERED(Dect2020MacMuxHeaderShortSduNoPayload);

Dect2020MacMuxHeaderShortSduNoPayload::Dect2020MacMuxHeaderShortSduNoPayload()
{

}

Dect2020MacMuxHeaderShortSduNoPayload::~Dect2020MacMuxHeaderShortSduNoPayload()
{
}

TypeId
Dect2020MacMuxHeaderShortSduNoPayload::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020MacMuxHeaderShortSduNoPayload")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020MacMuxHeaderShortSduNoPayload>();
    return tid;
}

TypeId
Dect2020MacMuxHeaderShortSduNoPayload::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
Dect2020MacMuxHeaderShortSduNoPayload::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;
    byte0 |= (m_macExtensionFieldEncoding & 0x03) << 6; // Bit 0-1
    byte0 |= (m_length & 0x01) << 5;                    // Bit 2
    byte0 |= (m_ieTypeFieldEncoding & 0x1F) << 0;       // Bit 3-7

    start.WriteU8(byte0);
}

uint32_t
Dect2020MacMuxHeaderShortSduNoPayload::Deserialize(Buffer::Iterator start)
{
    // Byte 0
    uint8_t byte0 = start.ReadU8();

    m_macExtensionFieldEncoding = (byte0 >> 6) & 0x03; // Bit 0-1
    m_length = (byte0 >> 5) & 0x01;                    // Bit 2
    m_ieTypeFieldEncoding = (byte0 & 0x1F);            // Bit 3-7

    return 1;
}

void
Dect2020MacMuxHeaderShortSduNoPayload::Print(std::ostream& os) const
{
    os << "Mac Extension Field Encoding: " << static_cast<int>(m_macExtensionFieldEncoding)
       << std::endl
       << "Length: " << static_cast<int>(m_length) << std::endl
       << "IE Type Field Encoding: " << static_cast<int>(m_ieTypeFieldEncoding) << std::endl;
}

uint32_t
Dect2020MacMuxHeaderShortSduNoPayload::GetSerializedSize(void) const
{
    return 1;
}

uint8_t
Dect2020MacMuxHeaderShortSduNoPayload::GetMacExtensionFieldEncoding() const
{
    return m_macExtensionFieldEncoding;
}

void
Dect2020MacMuxHeaderShortSduNoPayload::SetMacExtensionFieldEncoding(uint8_t encoding)
{
    m_ieTypeFieldEncoding = encoding;
}

uint8_t
Dect2020MacMuxHeaderShortSduNoPayload::GetLengthField() const
{
    return m_length;
}

void
Dect2020MacMuxHeaderShortSduNoPayload::SetLengthField(uint8_t length)
{
    m_length = length & 0x01;
}

IETypeFieldEncoding
Dect2020MacMuxHeaderShortSduNoPayload::GetIeTypeFieldEncoding() const
{
    return static_cast<IETypeFieldEncoding>(m_ieTypeFieldEncoding);
}



void
Dect2020MacMuxHeaderShortSduNoPayload::SetIeTypeFieldEncoding(IETypeFieldEncoding type)
{
    m_ieTypeFieldEncoding = static_cast<uint8_t>(type);
}

} // namespace ns3