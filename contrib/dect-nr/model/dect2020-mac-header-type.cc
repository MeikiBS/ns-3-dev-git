#include "dect2020-mac-header-type.h"

#include "dect2020-beacon-header.h"
#include "dect2020-beacon-message.h"

#include "ns3/log.h"

#include <algorithm> // für std::copy
#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020MacHeaderType");
NS_OBJECT_ENSURE_REGISTERED(Dect2020MacHeaderType);

TypeId
Dect2020MacHeaderType::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020MacHeaderType")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020MacHeaderType>();
    return tid;
}

TypeId
Dect2020MacHeaderType::GetInstanceTypeId() const
{
    return GetTypeId();
}

Dect2020MacHeaderType::Dect2020MacHeaderType()
    : m_version(0),
      m_macSecurity(MAC_SECURITY_NOT_USED),
      m_macHeaderTypeField(DATA_MAC_PDU_HEADER)
{
}

Dect2020MacHeaderType::~Dect2020MacHeaderType()
{
}

void
Dect2020MacHeaderType::SetVersion(uint8_t version)
{
    if (version != 0)
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Version must be 0.");
    }

    m_version = version;
}

uint8_t
Dect2020MacHeaderType::GetVersion() const
{
    return m_version;
}

void
Dect2020MacHeaderType::SetMacSecurity(MacSecurityField macSecurity)
{
    if ((macSecurity < 0) || (macSecurity > 3))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Mac Security must be between 0 and 3");
    }

    m_macSecurity = macSecurity;
}

ns3::Dect2020MacHeaderType::MacSecurityField
Dect2020MacHeaderType::GetMacSecurity() const
{
    return m_macSecurity;
}

void
Dect2020MacHeaderType::SetMacHeaderTypeField(MacHeaderTypeField macHeaderTypeField)
{
    if ((macHeaderTypeField < 0) || (macHeaderTypeField > 15))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Mac Header Type Field must be between 0 and 2^4-1");
    }

    m_macHeaderTypeField = macHeaderTypeField;
}

ns3::Dect2020MacHeaderType::MacHeaderTypeField
Dect2020MacHeaderType::GetMacHeaderTypeField() const
{
    return m_macHeaderTypeField;
}

uint32_t
Dect2020MacHeaderType::GetSerializedSize() const
{
    return 1;
}

void
Dect2020MacHeaderType::Print(std::ostream& os) const
{
    os << "Version = " << static_cast<int>(m_version) << std::endl
       << "MAC Security = " << static_cast<int>(m_macSecurity) << std::endl
       << "MAC Header Type = " << static_cast<int>(m_macHeaderTypeField) << std::endl;
}

void
Dect2020MacHeaderType::Serialize(Buffer::Iterator start) const
{
    // See ETSI Doc. 103 636-4 - V.1.5.1 - 6.3.2

    uint8_t byte0 = 0;

    byte0 |= (m_version & 0x03) << 6;       // Bit 0-1
    byte0 |= (m_macSecurity & 0x03) << 4;   // Bit 2-3
    byte0 |= (m_macHeaderTypeField & 0x0F); // Bit 4-7

    start.WriteU8(byte0);
}

uint32_t
Dect2020MacHeaderType::Deserialize(Buffer::Iterator start)
{
    // See ETSI Doc. 103 636-4 - V.1.5.1 - 6.3.2
    uint8_t bytesRead = 0;
    uint8_t byte0 = 0;

    byte0 = start.ReadU8();
    bytesRead++;

    m_version = (byte0 >> 6) & 0x03;
    m_macSecurity = static_cast<MacSecurityField>((byte0 >> 4) & 0x03);
    m_macHeaderTypeField = static_cast<MacHeaderTypeField>(byte0 & 0x0F);

    return bytesRead;
}

} // namespace ns3