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

uint32_t
Dect2020MacHeaderType::GetSerializedSize() const
{
    return 1;
}

void
Dect2020MacHeaderType::Print(std::ostream& os) const
{
    // os << "TX power = " << (bool)m_txPowerIncluded << std::endl
    //    << "Power const = " << (bool)m_powerConstraints << std::endl
    //    << "Current cluster channel = " << (bool)m_currentClusterChannelIncluded << std::endl
    //    << "Network Beacon channels = " << static_cast<int>(m_networkBeaconChannels) << std::endl
    //    << "Network Beacon period = " << static_cast<int>(m_networkBeaconPeriod) << std::endl
    //    << "Cluster Beacon period = " << static_cast<int>(m_clusterBeaconPeriod) << std::endl
    //    << "Next Cluster Channel = " << static_cast<uint16_t>(m_nextClusterChannel) << std::endl
    //    << "Time to Next = " << static_cast<uint32_t>(m_timeToNext) << std::endl
    //    << "Clusters Max TX Power = " << static_cast<int>(m_clustersMaxTxPower) << std::endl
    //    << "Current cluster channel " << static_cast<uint16_t>(m_currentClusterChannel) <<
    //    std::endl;
}

void
Dect2020MacHeaderType::Serialize(Buffer::Iterator start) const
{
    // See ETSI Doc. 103 636-4 - V.1.5.1 - 6.3.2

    uint8_t byte0 = 0;

    byte0 |= (version & 0x03) << 6;         // Bit 0-1
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
}

} // namespace ns3