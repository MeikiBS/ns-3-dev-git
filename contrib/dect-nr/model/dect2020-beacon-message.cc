#include "dect2020-beacon-message.h"

#include "dect2020-beacon-header.h"

#include "ns3/log.h"

#include <algorithm> // für std::copy
#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020BeaconMessage");
NS_OBJECT_ENSURE_REGISTERED(Dect2020BeaconMessage);

TypeId
Dect2020BeaconMessage::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020BeaconMessage")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020BeaconMessage>();
    return tid;
}

TypeId
Dect2020BeaconMessage::GetInstanceTypeId() const
{
    return GetTypeId();
}

Dect2020BeaconMessage::Dect2020BeaconMessage()
    : m_txPowerIncluded(false),
      m_powerConstraints(false),
      m_currentClusterChannelIncluded(false),
      m_networkBeaconChannels(0),
      m_networkBeaconPeriod(NETWORK_PERIOD_100MS),
      m_clusterBeaconPeriod(CLUSTER_PERIOD_10MS),
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

bool
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

/**
 * Set the number of additional Network Channels
 *
 * @param value Value between 0 and 3
 * @throws std::invalid_argument if the value is outside the valid range.
 */
void
Dect2020BeaconMessage::SetNetworkBeaconChannels(uint8_t networkBeaconChannels)
{
    if ((networkBeaconChannels < 0) || (networkBeaconChannels > 3))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Network Beacon Channels must be between 0 and 3");
    }

    m_networkBeaconChannels = networkBeaconChannels;
}

uint8_t
Dect2020BeaconMessage::GetNetworkBeaconChannels() const
{
    return m_networkBeaconChannels;
}

/**
 * Set the Network Beacon Period
 *
 * @param value Value between 0 and 2^4-1
 * @throws std::invalid_argument if the value is outside the valid range.
 */
void
Dect2020BeaconMessage::SetNetworkBeaconPeriod(NetworkBeaconPeriod networkBeaconPeriod)
{
    if ((networkBeaconPeriod < 0) || (networkBeaconPeriod > 15))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Network Beacon Period must be between 0 and 2^4-1");
    }

    m_networkBeaconPeriod = networkBeaconPeriod;
}

ns3::Dect2020BeaconMessage::NetworkBeaconPeriod
Dect2020BeaconMessage::GetNetworkBeaconPeriod() const
{
    return m_networkBeaconPeriod;
}

/**
 * Set the Cluster Beacon Period
 *
 * @param value Value between 0 and 2^4-1
 * @throws std::invalid_argument if the value is outside the valid range.
 */
void
Dect2020BeaconMessage::SetClusterBeaconPeriod(ClusterBeaconPeriod clusterBeaconPeriod)
{
    if ((clusterBeaconPeriod < 0) || (clusterBeaconPeriod > 15))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Cluster Beacon Period must be between 0 and 2^4-1");
    }

    m_clusterBeaconPeriod = clusterBeaconPeriod;
}

Dect2020BeaconMessage::ClusterBeaconPeriod
Dect2020BeaconMessage::GetClusterBeaconPeriod() const
{
    return m_clusterBeaconPeriod;
}

/**
 * Set the Next Cluster Channel
 *
 * @param value Value between 0 and 8191 (2^13-1)
 * @throws std::invalid_argument if the value is outside the valid range.
 */
void
Dect2020BeaconMessage::SetNextClusterChannel(uint16_t nextClusterChannel)
{
    if ((nextClusterChannel < 0) || (nextClusterChannel > 8191))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Next Cluster Channel must be between 0 and 8191");
    }

    m_nextClusterChannel = nextClusterChannel;
}

uint16_t
Dect2020BeaconMessage::GetNextClusterChannel() const
{
    return m_nextClusterChannel;
}

/**
 * Set the Time To Next
 *
 * @param value Value between 0 and 2^32-1
 * @throws std::invalid_argument if the value is outside the valid range.
 */
void
Dect2020BeaconMessage::SetTimeToNext(uint32_t timeToNext)
{
    if ((timeToNext < 0) || (timeToNext > UINT32_MAX))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Time To Next must be between 0 and 2^32-1");
    }

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
Dect2020BeaconMessage::SetAdditionalNetworkBeaconChannels(uint16_t* additionalNetworkBeaconChannels)
{
    std::copy(additionalNetworkBeaconChannels,
              additionalNetworkBeaconChannels + 3,
              m_additionalNetworkBeaconChannels);
    // m_additionalNetworkBeaconChannels = additionalNetworkBeaconChannels;
}

uint16_t*
Dect2020BeaconMessage::GetAdditionalNetworkBeaconChannels()
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
        sizeInBits += 4; // Clusters Max TX Power Field
    }

    if (m_currentClusterChannelIncluded == 1)
    {
        sizeInBits += 3;  // Reserved
        sizeInBits += 13; // Current Cluster Channel Field
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
    os << "TX power = " << (bool)m_txPowerIncluded << std::endl
       << "Power const = " << (bool)m_powerConstraints << std::endl
       << "Current cluster channel = " << (bool)m_currentClusterChannelIncluded << std::endl
       << "Network Beacon channels = " << static_cast<int>(m_networkBeaconChannels) << std::endl
       << "Network Beacon period = " << static_cast<int>(m_networkBeaconPeriod) << std::endl
       << "Cluster Beacon period = " << static_cast<int>(m_clusterBeaconPeriod) << std::endl
       << "Next Cluster Channel = " << static_cast<uint16_t>(m_nextClusterChannel) << std::endl
       << "Time to Next = " << static_cast<uint32_t>(m_timeToNext) << std::endl
       << "Clusters Max TX Power = " << static_cast<int>(m_clustersMaxTxPower) << std::endl
       << "Current cluster channel " << static_cast<uint16_t>(m_currentClusterChannel) << std::endl;

    // for (uint16_t channel : m_additionalNetworkBeaconChannels)
    // {
    //     os << "Additional Network Beacon Channel: " << static_cast<int>(channel) << std::endl;
    // }
    for (int idx = 0; idx < m_networkBeaconChannels; idx++)
    {
        os << "Additional Network Beacon Channel: "
           << static_cast<int>(m_additionalNetworkBeaconChannels[idx]) << std::endl;
    }
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
    byte2 |= (m_nextClusterChannel >> 8) & 0x1F; // Bit 3-7

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = 0;
    byte3 = m_nextClusterChannel & 0xFF; // Bit 0-7

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
        for (int idx = 0; idx < m_networkBeaconChannels; idx++)
        {
            start.WriteU16(m_additionalNetworkBeaconChannels[idx] & 0x1FFF);
        }
    }
}

uint32_t
Dect2020BeaconMessage::Deserialize(Buffer::Iterator start)
{
    // See ETSI Doc. 103 636-4 Figure 6.4.2.2-1 for more Information

    uint32_t bytesRead = 0;

    // Byte 0
    uint8_t byte0 = 0;

    byte0 = start.ReadU8();
    bytesRead++;

    // Bits 0-2: Reserved
    m_txPowerIncluded = (byte0 & (1 << 4)) != 0;               // Bit 3
    m_powerConstraints = (byte0 & (1 << 3)) != 0;              // Bit 4
    m_currentClusterChannelIncluded = (byte0 & (1 << 2)) != 0; // Bit 5
    m_networkBeaconChannels = byte0 & 0x03;                    // Bit 5-6

    // Byte 1
    uint8_t byte1 = 0;
    byte1 = start.ReadU8();
    bytesRead++;

    m_networkBeaconPeriod = static_cast<NetworkBeaconPeriod>((byte1 >> 4) & 0x0F); // Bit 0-3
    m_clusterBeaconPeriod = static_cast<ClusterBeaconPeriod>(byte1 & 0x0F);        // Bit 4-7

    // Byte 2
    uint8_t byte2 = 0;
    byte2 = start.ReadU8();
    bytesRead++;

    // Bit 0-2: Reserved
    // Bit 3-7: Next Cluster Channel
    uint16_t nextClusterChannelHigh = (byte2 & 0x1F) << 8;

    // Byte 3
    uint8_t byte3 = 0;
    byte3 = start.ReadU8();
    bytesRead++;

    uint16_t nextClusterChannelLow = byte3;
    m_nextClusterChannel = nextClusterChannelHigh | nextClusterChannelLow;

    // Byte 4-7: Time To next
    m_timeToNext = start.ReadNtohU32();
    bytesRead += 4;

    // Optional Fields
    if (m_txPowerIncluded)
    {
        // Byte 8
        uint8_t byte8 = 0;
        byte8 = start.ReadU8();
        bytesRead++;

        // Bit 0-3: Reserved
        m_clustersMaxTxPower = byte8 & 0x0F;
    }

    if (m_currentClusterChannelIncluded)
    {
        // Byte 9
        uint8_t byte9 = 0;
        byte9 = start.ReadU8();
        bytesRead++;

        uint16_t currentClusterChannelHigh = (byte9 & 0x1F) << 8;

        // Byte 10
        uint8_t byte10 = 0;
        byte10 = start.ReadU8();
        bytesRead++;

        m_currentClusterChannel = currentClusterChannelHigh | byte10;
    }

    if (m_networkBeaconChannels > 0)
    {
        for (int idx = 0; idx < m_networkBeaconChannels; idx++)
        {
            m_additionalNetworkBeaconChannels[idx] = start.ReadU16() & 0x1FFF;
            bytesRead += 2;
        }
    }

    return bytesRead;
}

} // namespace ns3