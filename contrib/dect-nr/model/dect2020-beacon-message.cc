#include "dect2020-beacon-message.h"

#include "dect2020-beacon-header.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020BeaconHeader");
NS_OBJECT_ENSURE_REGISTERED(Dect2020BeaconHeader);

TypeId
Dect2020BeaconMessage::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020BeaconMessage")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020BeaconMessage>();
    return tid;
}

Dect2020BeaconMessage::Dect2020BeaconMessage()
    : m_txPowerIncluded(false),
      m_powerConstraints(false),
      m_currentClusterChannelIncluded(false),
      m_networkBeaconChannels(0),
      m_networkBeaconPeriod(0),
      m_clusterBeaconPeriod(0),
      m_nextClusterChannel(0),
      m_timeToNext(0),
      m_clustersMaxTxPower(0),
      m_currenClusterChannel(0),
      m_additionalNetworkBeaconChannels(0)
{
}

Dect2020BeaconMessage::~Dect2020BeaconMessage()
{
}

void
Dect2020BeaconMessage::SetTxPowerIncluded(bool txPowerIncluded)
{
    m_txPowerIncluded = txPowerIncluded;
}

void
Dect2020BeaconMessage::GetTxPowerIncluded()
{
    return m_txPowerIncluded;
}

void
Dect2020BeaconMessage::SetPowerConstraints(bool powerConstraints)
{
    m_powerConstraints = powerConstraints;
}

bool
Dect2020BeaconMessage::GetPowerConstraints()
{
    return m_powerConstraints;
}

void
Dect2020BeaconMessage::SetCurrentClusterChannelIncluded(bool currentClusterChannelIncluded)
{
    m_currentClusterChannelIncluded = currentClusterChannelIncluded;
}

bool
Dect2020BeaconMessage::GetCurrentClusterChannelIncluded()
{
    return m_currentClusterChannelIncluded;
}

void
Dect2020BeaconMessage::SetNetworkBeaconChannels(uint8_t networkBeaconChannels)
{
    m_networkBeaconChannels = networkBeaconChannels;
}

void
Dect2020BeaconMessage::GetNetworkBeaconChannels()
{
    return m_networkBeaconChannels;
}

} // namespace ns3