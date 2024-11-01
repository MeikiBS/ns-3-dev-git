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
      m_currentClusterChannel(0),
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
Dect2020BeaconMessage::GetTxPowerIncluded() const
{
    return m_txPowerIncluded;
}

void
Dect2020BeaconMessage::SetPowerConstraints(bool powerConstraints)
{
    m_powerConstraints = powerConstraints;
}

bool
Dect2020BeaconMessage::GetPowerConstraints() const
{
    return m_powerConstraints;
}

void
Dect2020BeaconMessage::SetCurrentClusterChannelIncluded(bool currentClusterChannelIncluded)
{
    m_currentClusterChannelIncluded = currentClusterChannelIncluded;
}

bool
Dect2020BeaconMessage::GetCurrentClusterChannelIncluded() const
{
    return m_currentClusterChannelIncluded;
}

void
Dect2020BeaconMessage::SetNetworkBeaconChannels(uint8_t networkBeaconChannels)
{
    m_networkBeaconChannels = networkBeaconChannels;
}

uint8_t
Dect2020BeaconMessage::GetNetworkBeaconChannels() const
{
    return m_networkBeaconChannels;
}

void
Dect2020BeaconMessage::SetNetworkBeaconPeriod(uint8_t networkBeaconPeriod)
{
    m_networkBeaconPeriod = networkBeaconPeriod;
}

uint8_t
Dect2020BeaconMessage::GetNetworkBeaconPeriod() const
{
    return m_networkBeaconPeriod;
}

void
Dect2020BeaconMessage::SetClusterBeaconPeriod(uint8_t clusterBeaconPeriod)
{
    m_clusterBeaconPeriod = clusterBeaconPeriod;
}

uint8_t
Dect2020BeaconMessage::GetClusterBeaconPeriod() const
{
    return m_clusterBeaconPeriod;
}

void
Dect2020BeaconMessage::SetNextClusterChannel(uint16_t nextClusterChannel)
{
    m_nextClusterChannel = nextClusterChannel;
}

uint16_t
Dect2020BeaconMessage::GetNextClusterChannel() const
{
    return m_nextClusterChannel;
}

void
Dect2020BeaconMessage::SetTimeToNext(uint32_t timeToNext)
{
    m_timeToNext = timeToNext;
}

uint32_t
Dect2020BeaconMessage::GetTimeToNext() const
{
    return m_timeToNext;
}

void
Dect2020BeaconMessage::SetClustersMaxTxPower(uint8_t clustersMaxTxPower)
{
    m_clustersMaxTxPower = clustersMaxTxPower;
}

uint8_t
Dect2020BeaconMessage::GetClustersMaxTxPower() const
{
    return m_clustersMaxTxPower;
}

void
Dect2020BeaconMessage::SetCurrentClusterChannel(uint16_t currentClusterChannel)
{
    m_currentClusterChannel = currentClusterChannel;
}

uint16_t
Dect2020BeaconMessage::GetCurrentClusterChannel() const
{
    return m_currentClusterChannel;
}

void
Dect2020BeaconMessage::SetAdditionalNetworkBeaconChannels(uint16_t additionalNetworkBeaconChannels)
{
    m_additionalNetworkBeaconChannels = additionalNetworkBeaconChannels;
}

uint16_t
Dect2020BeaconMessage::GetAdditionalNetworkBeaconChannels() const
{
    return m_additionalNetworkBeaconChannels;
}

uint32_t
Dect2020BeaconMessage::GetSerializedSize() const
{
    uint32_t sizeInBits = 0;

    sizeInBits += 64; // Byte 0 - Byte 7

    if (m_txPowerIncluded == 1)
    {
        sizeInBits += 4; // Reserved
        sizeInBits += 8; // Clusters Max TX Power Field
    }

    if (m_currentClusterChannelIncluded == 1)
    {
        sizeInBits += 3;  // Reserved
        sizeInBits += 16; // Current Cluster Channel Field
    }

    if (m_networkBeaconChannels > 0)
    {
        sizeInBits += m_networkBeaconChannels * 3;  // Reserved
        sizeInBits += m_networkBeaconChannels * 13; // Additional Network Beacon Channels
    }

    // Round up to the next multiple of 8.
    uint32_t sizeInBytes = (sizeInBits + 7) / 8;

    return sizeInBytes;
}

void
Dect2020BeaconMessage::Print(std::ostream& os) const
{
    os << "TX power = " << (bool)m_txPowerIncluded << "Power const = " << (bool)m_powerConstraints
       << "Current cluster channel = " << (bool)m_currentClusterChannelIncluded
       << "Network Beacon channels = " << (uint8_t)m_networkBeaconChannels
       << "Network Beacon period = " << (uint8_t)m_networkBeaconPeriod
       << "Cluster Beacon period = " << (uint8_t)m_clusterBeaconPeriod
       << "Next Cluster Channel = " << (uint16_t)m_nextClusterChannel
       << "Time to Next = " << (uint32_t)m_timeToNext
       << "Clusters Max TX Power = " << (uint8_t)m_clustersMaxTxPower << "Current cluster channel "
       << (uint16_t)m_currentClusterChannel << "Additional Network Beacon Channels "
       << (uint16_t)m_additionalNetworkBeaconChannels;
}

void
Dect2020BeaconMessage::Serialize(Buffer::Iterator start) const
{
    // See ETSI Doc. 103 636-4 Figure 6.4.2.2-1 for more Information

    // Byte 0
    uint8_t byte0 = 0;
    // Bit 0-2 Reserved
    byte0 |= (m_txPowerIncluded << 4);               // Bit 3
    byte0 |= (m_powerConstraints << 3);              // Bit 4
    byte0 |= (m_currentClusterChannelIncluded << 2); // Bit 5
    byte0 |= (m_networkBeaconChannels & 0x03);       // Bit 6-7

    start.WriteU8(byte0);

    // Byte 1
    uint8_t byte1 = 0;
    byte1 |= (m_networkBeaconPeriod & 0x0F) << 4; // Bit 0-3
    byte1 |= (m_clusterBeaconPeriod & 0x0F);      // Bit 4-7

    start.WriteU8(byte1);

    // Byte 2
    uint8_t byte2 = 0;
    byte1 |= (m_nextClusterChannel & 0x1F); // Bit 3-7

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = 0;
    byte3 = m_nextClusterChannel; // Bit 0-7

    start.WriteU8(byte3);

    // Byte 4-7
    start.WriteHtonU32(m_timeToNext);

    if (m_txPowerIncluded)
    {
        // Byte 8
        uint8_t byte8 = 0;
        // Bit 0-3 Reserved
        byte8 |= (m_clustersMaxTxPower & 0x0F); // Bit 4-7

        start.WriteU8(byte8);
    }

    if (m_currentClusterChannelIncluded)
    {
        // Byte 9
        uint8_t byte9 = 0;
        // Bit 0-2 Reserved
        byte9 |= (m_currentClusterChannel >> 8) & 0x1F; // Bit 3-7

        start.WriteU8(byte9);

        // Byte 10
        uint8_t byte10 = 0;
        byte10 |= (m_currentClusterChannel & 0xFF); // Bit 0-7

        start.WriteU8(byte10);
    }

    if (m_networkBeaconChannels > 0)
    {
        // Byte 11
        uint8_t byte11 = 0;
        // Bit 0-2 Reserved
        byte11 |= (m_additionalNetworkBeaconChannels >> 8) & 0x1F; // Bit 3-7

        start.WriteU8(byte11);

        // Byte 12
        uint8_t byte12 = 0;

        byte12 |= (m_additionalNetworkBeaconChannels & 0xFF); // Bit 0-7
        start.WriteU8(byte11);
    }
}

uint32_t
Dect2020BeaconMessage::Deserialize(Buffer::Iterator start)
{
}

} // namespace ns3