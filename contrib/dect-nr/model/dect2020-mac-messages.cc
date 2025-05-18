#include "dect2020-mac-messages.h"

#include "dect2020-mac-common-header.h"

#include "ns3/log.h"

#include <algorithm> // für std::copy
#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020BeaconMessage");
NS_OBJECT_ENSURE_REGISTERED(Dect2020ClusterBeaconMessage);

// *******************************************************
//            DECT2020 Cluster Beacon Message
// *******************************************************

const std::array<int8_t, 16> txPowerTable = {
    0,   // 0000 = reserved
    0,   // 0001 = reserved
    0,   // 0010 = reserved
    0,   // 0011 = reserved
    -12, // 0100
    -8,  // 0101
    -4,  // 0110
    0,   // 0111
    4,   // 1000
    7,   // 1001
    10,  // 1010
    13,  // 1011
    16,  // 1100
    19,  // 1101
    21,  // 1110
    23   // 1111
};

int8_t
Dect2020ClusterBeaconMessage::GetTxPowerFromField(uint8_t field)
{
    if (field < txPowerTable.size())
    {
        return txPowerTable[field];
    }
    return 0; // fallback
}

TypeId
Dect2020ClusterBeaconMessage::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020ClusterBeaconMessage")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020ClusterBeaconMessage>();
    return tid;
}

TypeId
Dect2020ClusterBeaconMessage::GetInstanceTypeId() const
{
    return GetTypeId();
}

Dect2020ClusterBeaconMessage::Dect2020ClusterBeaconMessage()
    : m_SFN(0),
      m_txPowerIncluded(false),
      m_powerConstraints(false),
      m_FO(false),
      m_nextChannel(false),
      m_timeToNextFieldPresent(false),
      m_networkBeaconPeriod(NETWORK_PERIOD_50MS),
      m_clusterBeaconPeriod(CLUSTER_PERIOD_10MS),
      m_countToTrigger(0),
      m_relQuality(0),
      m_minQuality(0),
      m_clusterMaxTxPower(GetTxPowerFromField(4)),
      m_frameOffset(0),
      m_nextClusterChannel(0),
      m_timeToNext(0)
{
}

Dect2020ClusterBeaconMessage::~Dect2020ClusterBeaconMessage()
{
}

void
Dect2020ClusterBeaconMessage::SetSystemFrameNumber(uint8_t sfn)
{
    m_SFN = sfn;
}

uint8_t
Dect2020ClusterBeaconMessage::GetSystemFrameNumber() const
{
    return m_SFN;
}

void
Dect2020ClusterBeaconMessage::SetTxPowerIncluded(bool txPowerIncluded)
{
    m_txPowerIncluded = txPowerIncluded;
}

bool
Dect2020ClusterBeaconMessage::GetTxPowerIncluded() const
{
    return m_txPowerIncluded;
}

void
Dect2020ClusterBeaconMessage::SetPowerConstraints(bool powerConstraints)
{
    m_powerConstraints = powerConstraints;
}

bool
Dect2020ClusterBeaconMessage::GetPowerConstraints() const
{
    return m_powerConstraints;
}

void
Dect2020ClusterBeaconMessage::SetFrameOffsetIncluded(bool frameOffsetIncluded)
{
    m_FO = frameOffsetIncluded;
}

bool
Dect2020ClusterBeaconMessage::GetFrameOffsetIncluded() const
{
    return m_FO;
}

void
Dect2020ClusterBeaconMessage::SetNextChannelIncluded(bool nextChannelIncluded)
{
    m_nextChannel = nextChannelIncluded;
}

bool
Dect2020ClusterBeaconMessage::GetNextChannelIncluded() const
{
    return m_nextChannel;
}

void
Dect2020ClusterBeaconMessage::SetTimeToNextFieldPresent(bool timeToNextFieldPresent)
{
    m_timeToNextFieldPresent = timeToNextFieldPresent;
}

bool
Dect2020ClusterBeaconMessage::GetTimeToNextFieldPresent() const
{
    return m_timeToNextFieldPresent;
}

void
Dect2020ClusterBeaconMessage::SetNetworkBeaconPeriod(NetworkBeaconPeriod period)
{
    if (period > 15)
    {
        throw std::invalid_argument("Network Beacon Period must be between 0 and 15");
    }
    m_networkBeaconPeriod = period;
}

NetworkBeaconPeriod
Dect2020ClusterBeaconMessage::GetNetworkBeaconPeriod() const
{
    return m_networkBeaconPeriod;
}

void
Dect2020ClusterBeaconMessage::SetClusterBeaconPeriod(ClusterBeaconPeriod period)
{
    if (period > 15)
    {
        throw std::invalid_argument("Cluster Beacon Period must be between 0 and 15");
    }
    m_clusterBeaconPeriod = period;
}

ClusterBeaconPeriod
Dect2020ClusterBeaconMessage::GetClusterBeaconPeriod() const
{
    return m_clusterBeaconPeriod;
}

void
Dect2020ClusterBeaconMessage::SetCountToTrigger(uint8_t count)
{
    m_countToTrigger = count;
}

uint8_t
Dect2020ClusterBeaconMessage::GetCountToTrigger() const
{
    return m_countToTrigger;
}

void
Dect2020ClusterBeaconMessage::SetRelativeQuality(uint8_t relQuality)
{
    m_relQuality = relQuality;
}

uint8_t
Dect2020ClusterBeaconMessage::GetRelativeQuality() const
{
    return m_relQuality;
}

void
Dect2020ClusterBeaconMessage::SetMinimumQuality(uint8_t minQuality)
{
    m_minQuality = minQuality;
}

uint8_t
Dect2020ClusterBeaconMessage::GetMinimumQuality() const
{
    return m_minQuality;
}

void
Dect2020ClusterBeaconMessage::SetClusterMaxTxPower(uint8_t power)
{
    m_clusterMaxTxPower = power;
}

uint8_t
Dect2020ClusterBeaconMessage::GetClusterMaxTxPower() const
{
    return m_clusterMaxTxPower;
}

void
Dect2020ClusterBeaconMessage::SetFrameOffset(uint16_t offset)
{
    m_frameOffset = offset;
}

uint16_t
Dect2020ClusterBeaconMessage::GetFrameOffset() const
{
    return m_frameOffset;
}

void
Dect2020ClusterBeaconMessage::SetNextClusterChannel(uint16_t channel)
{
    m_nextClusterChannel = channel;
}

uint16_t
Dect2020ClusterBeaconMessage::GetNextClusterChannel() const
{
    return m_nextClusterChannel;
}

void
Dect2020ClusterBeaconMessage::SetTimeToNext(uint32_t time)
{
    m_timeToNext = time;
}

uint32_t
Dect2020ClusterBeaconMessage::GetTimeToNext() const
{
    return m_timeToNext;
}

void
Dect2020ClusterBeaconMessage::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;
    byte0 = m_SFN; // Bit 0-7

    start.WriteU8(byte0);

    // Byte 1
    uint8_t byte1 = 0;
    // Bit 0-2 Reserved
    byte1 |= (m_txPowerIncluded << 4);   // Bit 3
    byte1 |= (m_powerConstraints << 3);  // Bit 4
    byte1 |= (m_FO << 2);                // Bit 5
    byte1 |= (m_nextChannel << 1);       // Bit 6
    byte1 |= (m_timeToNextFieldPresent); // Bit 7

    start.WriteU8(byte1);

    // Byte 2
    uint8_t byte2 = 0;
    byte2 |= (m_networkBeaconPeriod & 0x0F) << 4; // Bit 0-3
    byte2 |= (m_clusterBeaconPeriod & 0x0F);      // Bit 4-7

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = 0;
    byte3 |= (m_countToTrigger & 0x0F) << 4; // Bit 0-3
    byte3 |= (m_relQuality & 0x03) << 2;     // Bit 4-5
    byte3 |= (m_minQuality & 0x03);          // Bit 6-7

    start.WriteU8(byte3);

    if (m_txPowerIncluded)
    {
        // Byte 4
        uint8_t byte4 = 0;
        // Bit 0-2 Reserved
        byte4 |= (m_clusterMaxTxPower & 0x0F); // Bit 4-7

        start.WriteU8(byte4);
    }

    if (m_FO) // Note: Only the 8 bit variant is implemented. For subcarrier scaling factor >= 4,
              // Frame Offset is 16 Bit
    {
        // Byte 5
        uint8_t byte5 = 0;
        byte5 = m_frameOffset; // Bit 0-7

        start.WriteU8(byte5);
    }

    if (m_nextChannel)
    {
        // Byte 6
        uint8_t byte6 = 0;
        byte6 |= (m_nextClusterChannel >> 8) & 0x1F; // Bit 0-7

        start.WriteU8(byte6);

        // Byte 7
        uint8_t byte7 = 0;
        byte7 |= (m_nextClusterChannel & 0xFF); // Bit 0-7

        start.WriteU8(byte7);
    }

    if (m_timeToNext)
    {
        start.WriteU32(m_timeToNext);
    }
}

uint32_t
Dect2020ClusterBeaconMessage::Deserialize(Buffer::Iterator start)
{
    uint32_t bytesRead = 0;

    // Byte 0
    uint8_t byte0 = start.ReadU8();
    bytesRead++;
    m_SFN = byte0;

    // Byte 1
    uint8_t byte1 = start.ReadU8();
    bytesRead++;
    m_txPowerIncluded = (byte1 >> 4) & 0x01;
    m_powerConstraints = (byte1 >> 3) & 0x01;
    m_FO = (byte1 >> 2) & 0x01;
    m_nextChannel = (byte1 >> 1) & 0x01;
    m_timeToNextFieldPresent = (byte1 >> 0) & 0x01;

    // Byte 2
    uint8_t byte2 = start.ReadU8();
    bytesRead++;
    m_networkBeaconPeriod = static_cast<NetworkBeaconPeriod>((byte2 >> 4) & 0x0F);
    m_clusterBeaconPeriod = static_cast<ClusterBeaconPeriod>(byte2 & 0x0F);

    // Byte 3
    uint8_t byte3 = start.ReadU8();
    bytesRead++;
    m_countToTrigger = (byte3 >> 4) & 0x0F;
    m_relQuality = (byte3 >> 2) & 0x03;
    m_minQuality = byte3 & 0x03;

    // Byte 4 (optional)
    if (m_txPowerIncluded)
    {
        uint8_t byte4 = start.ReadU8();
        bytesRead++;
        m_clusterMaxTxPower = byte4 & 0x0F;
    }

    // Byte 5 (optional)
    if (m_FO)
    {
        uint8_t byte5 = start.ReadU8();
        bytesRead++;
        m_frameOffset = byte5; // only 8 Bit variant implemented
    }

    // Bytes 6–7 (optional)
    if (m_nextChannel)
    {
        uint8_t byte6 = start.ReadU8();
        bytesRead++;
        uint8_t byte7 = start.ReadU8();
        bytesRead++;
        m_nextClusterChannel = ((byte6 & 0x1F) << 8) | byte7;
    }

    // Bytes 8–11 (optional)
    if (m_timeToNextFieldPresent)
    {
        m_timeToNext = start.ReadU32();
        bytesRead += 4;
    }

    return bytesRead;
}

uint32_t
Dect2020ClusterBeaconMessage::GetSerializedSize() const
{
    uint32_t sizeInBits = 0;

    sizeInBits += 8; // Byte 0: SFN
    sizeInBits += 8; // Byte 1: Flags
    sizeInBits += 8; // Byte 2: Network/Cluster Beacon Period
    sizeInBits += 8; // Byte 3: CountToTrigger, RelQuality, MinQuality

    if (m_txPowerIncluded)
    {
        sizeInBits += 8; // Byte 4: Cluster Max TX Power + reserved
    }

    if (m_FO)
    {
        sizeInBits += 8; // Byte 5: FrameOffset (8 Bit variant)
    }

    if (m_nextChannel)
    {
        sizeInBits += 16; // Bytes 6–7: Next Cluster Channel (13 bits + reserved)
    }

    if (m_timeToNextFieldPresent)
    {
        sizeInBits += 32; // Bytes 8–11: Time to Next (32 bits)
    }

    // Round up to full bytes
    uint32_t sizeInBytes = (sizeInBits + 7) / 8;
    return sizeInBytes;
}

void
Dect2020ClusterBeaconMessage::Print(std::ostream& os) const
{
    os << "Cluster Beacon Message:" << std::endl
       << "System Frame Number (SFN): " << static_cast<int>(m_SFN) << std::endl
       << "TX Power Included: " << std::boolalpha << m_txPowerIncluded << std::endl
       << "Power Constraints: " << std::boolalpha << m_powerConstraints << std::endl
       << "Frame Offset Included (FO): " << std::boolalpha << m_FO << std::endl
       << "Next Channel Included: " << std::boolalpha << m_nextChannel << std::endl
       << "TimeToNext Field Present: " << std::boolalpha << m_timeToNextFieldPresent << std::endl
       << "Network Beacon Period: " << static_cast<int>(m_networkBeaconPeriod) << std::endl
       << "Cluster Beacon Period: " << static_cast<int>(m_clusterBeaconPeriod) << std::endl
       << "Count to Trigger: " << static_cast<int>(m_countToTrigger) << std::endl
       << "Relative Quality: " << static_cast<int>(m_relQuality) << std::endl
       << "Minimum Quality: " << static_cast<int>(m_minQuality) << std::endl;

    if (m_txPowerIncluded)
    {
        os << "Cluster Max TX Power: " << static_cast<int>(m_clusterMaxTxPower) << " dBm"
           << std::endl;
    }

    if (m_FO)
    {
        os << "Frame Offset: " << static_cast<uint16_t>(m_frameOffset) << std::endl;
    }

    if (m_nextChannel)
    {
        os << "Next Cluster Channel: " << static_cast<uint16_t>(m_nextClusterChannel) << std::endl;
    }

    if (m_timeToNextFieldPresent)
    {
        os << "Time to Next: " << static_cast<uint32_t>(m_timeToNext) << " µs" << std::endl;
    }
}

// *******************************************************
//            DECT2020 Network Beacon Message
// *******************************************************

// NS_LOG_COMPONENT_DEFINE("Dect2020NetworkBeaconMessage");
NS_OBJECT_ENSURE_REGISTERED(Dect2020NetworkBeaconMessage);

TypeId
Dect2020NetworkBeaconMessage::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020NetworkBeaconMessage")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020NetworkBeaconMessage>();
    return tid;
}

TypeId
Dect2020NetworkBeaconMessage::GetInstanceTypeId() const
{
    return GetTypeId();
}

Dect2020NetworkBeaconMessage::Dect2020NetworkBeaconMessage()
    : m_txPowerIncluded(false),
      m_powerConstraints(false),
      m_currentClusterChannelIncluded(false),
      m_networkBeaconChannels(0),
      m_networkBeaconPeriod(NETWORK_PERIOD_500MS),
      m_clusterBeaconPeriod(CLUSTER_PERIOD_1000MS),
      m_nextClusterChannel(0),
      m_timeToNext(0),
      m_clustersMaxTxPower(0),
      m_currentClusterChannel(0),
      m_additionalNetworkBeaconChannels(0)
{
}

Dect2020NetworkBeaconMessage::~Dect2020NetworkBeaconMessage()
{
}

void
Dect2020NetworkBeaconMessage::SetTxPowerIncluded(bool txPowerIncluded)
{
    m_txPowerIncluded = txPowerIncluded;
}

bool
Dect2020NetworkBeaconMessage::GetTxPowerIncluded() const
{
    return m_txPowerIncluded;
}

void
Dect2020NetworkBeaconMessage::SetPowerConstraints(bool powerConstraints)
{
    m_powerConstraints = powerConstraints;
}

bool
Dect2020NetworkBeaconMessage::GetPowerConstraints() const
{
    return m_powerConstraints;
}

void
Dect2020NetworkBeaconMessage::SetCurrentClusterChannelIncluded(bool currentClusterChannelIncluded)
{
    m_currentClusterChannelIncluded = currentClusterChannelIncluded;
}

bool
Dect2020NetworkBeaconMessage::GetCurrentClusterChannelIncluded() const
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
Dect2020NetworkBeaconMessage::SetNetworkBeaconChannels(uint8_t networkBeaconChannels)
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
Dect2020NetworkBeaconMessage::GetNetworkBeaconChannels() const
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
Dect2020NetworkBeaconMessage::SetNetworkBeaconPeriod(NetworkBeaconPeriod networkBeaconPeriod)
{
    if ((networkBeaconPeriod < 0) || (networkBeaconPeriod > 15))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Network Beacon Period must be between 0 and 2^4-1");
    }

    m_networkBeaconPeriod = networkBeaconPeriod;
}

NetworkBeaconPeriod
Dect2020NetworkBeaconMessage::GetNetworkBeaconPeriod() const
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
Dect2020NetworkBeaconMessage::SetClusterBeaconPeriod(ClusterBeaconPeriod clusterBeaconPeriod)
{
    if ((clusterBeaconPeriod < 0) || (clusterBeaconPeriod > 15))
    {
        throw std::invalid_argument("Error in " + std::string(__func__) + " at " +
                                    std::string(__FILE__) + ":" + std::to_string(__LINE__) +
                                    " - Cluster Beacon Period must be between 0 and 2^4-1");
    }

    m_clusterBeaconPeriod = clusterBeaconPeriod;
}

ClusterBeaconPeriod
Dect2020NetworkBeaconMessage::GetClusterBeaconPeriod() const
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
Dect2020NetworkBeaconMessage::SetNextClusterChannel(uint16_t nextClusterChannel)
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
Dect2020NetworkBeaconMessage::GetNextClusterChannel() const
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
Dect2020NetworkBeaconMessage::SetTimeToNext(uint32_t timeToNext)
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
Dect2020NetworkBeaconMessage::GetTimeToNext() const
{
    return m_timeToNext;
}

void
Dect2020NetworkBeaconMessage::SetClustersMaxTxPower(uint8_t clustersMaxTxPower)
{
    m_clustersMaxTxPower = clustersMaxTxPower;
}

uint8_t
Dect2020NetworkBeaconMessage::GetClustersMaxTxPower() const
{
    return m_clustersMaxTxPower;
}

void
Dect2020NetworkBeaconMessage::SetCurrentClusterChannel(uint16_t currentClusterChannel)
{
    m_currentClusterChannel = currentClusterChannel;
}

uint16_t
Dect2020NetworkBeaconMessage::GetCurrentClusterChannel() const
{
    return m_currentClusterChannel;
}

void
Dect2020NetworkBeaconMessage::SetAdditionalNetworkBeaconChannels(
    uint16_t* additionalNetworkBeaconChannels)
{
    if (additionalNetworkBeaconChannels != nullptr)
    {
        std::copy(additionalNetworkBeaconChannels,
                  additionalNetworkBeaconChannels + 3,
                  m_additionalNetworkBeaconChannels);
    }
    else
    {
        NS_LOG_WARN("SetAdditionalNetworkBeaconChannels() received nullptr!");
    }

    // m_additionalNetworkBeaconChannels = additionalNetworkBeaconChannels;
}

uint16_t*
Dect2020NetworkBeaconMessage::GetAdditionalNetworkBeaconChannels()
{
    return m_additionalNetworkBeaconChannels;
}

uint32_t
Dect2020NetworkBeaconMessage::GetSerializedSize() const
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
Dect2020NetworkBeaconMessage::Print(std::ostream& os) const
{
    os << "Beacon Message:" << std::endl
       << "TX power = " << (bool)m_txPowerIncluded << std::endl
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
Dect2020NetworkBeaconMessage::Serialize(Buffer::Iterator start) const
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
Dect2020NetworkBeaconMessage::Deserialize(Buffer::Iterator start)
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

uint16_t
Dect2020NetworkBeaconMessage::GetNetworkBeaconPeriodTime() const
{
    switch (m_networkBeaconPeriod)
    {
    case NETWORK_PERIOD_50MS:
        return 50;
        break;

    case NETWORK_PERIOD_100MS:
        return 100;
        break;

    case NETWORK_PERIOD_500MS:
        return 500;
        break;

    case NETWORK_PERIOD_1000MS:
        return 1000;
        break;

    case NETWORK_PERIOD_1500MS:
        return 1500;
        break;

    case NETWORK_PERIOD_2000MS:
        return 2000;
        break;

    case NETWORK_PERIOD_4000MS:
        return 4000;
        break;

    default:
        NS_FATAL_ERROR("Invalid network beacon period. Supported periods are 50, 100, 500, "
                       "1000, 1500, 2000, and 4000 ms.");
    }
}


// *******************************************************
//            DECT2020 Association Request Message
//            # ETSI TS 103 636-4 V2.1.1 6.4.2.4
// *******************************************************

Dect2020AssociationRequestMessage::Dect2020AssociationRequestMessage()
{
}

Dect2020AssociationRequestMessage::~Dect2020AssociationRequestMessage()
{
}

TypeId
Dect2020AssociationRequestMessage::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020AssociationRequestMessage")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020AssociationRequestMessage>();
    return tid;
}

TypeId
Dect2020AssociationRequestMessage::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
Dect2020AssociationRequestMessage::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;
    byte0 |= (m_setupCause & 0x07) << 5;    // Bit 0-2
    byte0 |= (m_numberOfFlows & 0x07) << 2; // Bit 3-5
    byte0 |= (m_powerConstraints << 1);     // Bit 6
    byte0 |= (m_ftMode << 0);               // Bit 7

    start.WriteU8(byte0);

    // Byte 1
    uint8_t byte1 = 0;
    byte1 |= (m_current << 7); // Bit 0
    // Bit 1-7 Reserved

    start.WriteU8(byte1);

    // Byte 2
    uint8_t byte2 = 0;
    byte2 |= (m_harqProcessesTx & 0x07) << 5;  // Bit 0-2
    byte2 |= (m_maxHarqReTxDelay & 0x1F) << 0; // Bit 3-7

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = 0;
    byte3 |= (m_harqProcessesRx & 0x07) << 5;  // Bit 0-2
    byte3 |= (m_maxHarqReRxDelay & 0x1F) << 0; // Bit 3-7

    start.WriteU8(byte3);

    // Byte 4
    uint8_t byte4 = 0;
    // Bit 0-1 Reserved
    byte4 |= (m_flowId & 0x3F); // Bit 2-7

    start.WriteU8(byte4);

    if (m_ftMode == 1)
    {
        // Byte 5
        uint8_t byte5 = 0;
        byte5 |= (m_networkBeaconPeriod & 0x0F) << 4; // Bit 0-3
        byte5 |= (m_clusterBeaconPeriod & 0x0F);      // Bit 4-7

        start.WriteU8(byte5);

        // Byte 6
        uint8_t byte6 = 0;
        // Bit 0-2 Reserved
        byte6 |= (m_nextClusterChannel >> 8) & 0x1F; // Bit 3-7

        start.WriteU8(byte6);

        // Byte 7
        uint8_t byte7 = 0;
        byte7 |= (m_nextClusterChannel & 0xFF); // Bit 0-7

        start.WriteU8(byte7);

        // Byte 8 - 11
        start.WriteHtonU32(m_timeToNext);
    }

    if (m_current == 1)
    {
        // Byte 12
        uint8_t byte12 = 0;
        byte12 |= (m_currentClusterChannel >> 8) & 0x1F; // Bit 3-7

        start.WriteU8(byte12);

        // Byte 13
        uint8_t byte13 = 0;
        byte13 |= (m_currentClusterChannel & 0xFF); // Bit 0-7

        start.WriteU8(byte13);
    }
}

void
Dect2020AssociationRequestMessage::Print(std::ostream& os) const
{
    os << "Dect2020AssociationRequestMessage:" << std::endl;
    os << "  Setup Cause: " << static_cast<uint32_t>(m_setupCause) << std::endl;
    os << "  Number of Flows: " << static_cast<uint32_t>(m_numberOfFlows) << std::endl;
    os << "  Power Constraints: " << std::boolalpha << m_powerConstraints << std::endl;
    os << "  FT Mode: " << std::boolalpha << m_ftMode << std::endl;
    os << "  Current: " << std::boolalpha << m_current << std::endl;
    os << "  HARQ Processes TX: " << static_cast<uint32_t>(m_harqProcessesTx) << std::endl;
    os << "  Max HARQ ReTX Delay: " << static_cast<uint32_t>(m_maxHarqReTxDelay) << std::endl;
    os << "  HARQ Processes RX: " << static_cast<uint32_t>(m_harqProcessesRx) << std::endl;
    os << "  Max HARQ ReRX Delay: " << static_cast<uint32_t>(m_maxHarqReRxDelay) << std::endl;
    os << "  Flow ID: " << static_cast<uint32_t>(m_flowId) << std::endl;

    if (m_ftMode)
    {
        os << "  Network Beacon Period: " << static_cast<uint32_t>(m_networkBeaconPeriod)
           << std::endl;
        os << "  Cluster Beacon Period: " << static_cast<uint32_t>(m_clusterBeaconPeriod)
           << std::endl;
        os << "  Next Cluster Channel: " << m_nextClusterChannel << std::endl;
        os << "  Time To Next: " << m_timeToNext << std::endl;
    }

    if (m_current)
    {
        os << "  Current Cluster Channel: " << m_currentClusterChannel << std::endl;
    }
}

uint32_t
Dect2020AssociationRequestMessage::GetSerializedSize() const
{
    uint32_t size = 0;

    size += 5; // Byte 0–4 (immer vorhanden)

    if (m_ftMode)
    {
        size += 1; // Byte 5: Beacon Periods
        size += 2; // Byte 6–7: Next Cluster Channel
        size += 4; // Byte 8–11: Time To Next
    }

    if (m_current)
    {
        size += 2; // Byte 12–13: Current Cluster Channel
    }

    return size;
}

uint32_t
Dect2020AssociationRequestMessage::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    // Byte 0
    uint8_t byte0 = i.ReadU8();
    SetSetupCause((byte0 >> 5) & 0x07);       // Bit 0-2
    SetNumberOfFlows((byte0 >> 2) & 0x07);    // Bit 3-5
    SetPowerConstraints((byte0 >> 1) & 0x01); // Bit 6
    SetFtMode(byte0 & 0x01);                  // Bit 7

    // Byte 1
    uint8_t byte1 = i.ReadU8();
    SetCurrent((byte1 >> 7) & 0x01); // Bit 0
    // Bit 1-7 Reserved

    // Byte 2
    uint8_t byte2 = i.ReadU8();
    SetHarqProcessesTx((byte2 >> 5) & 0x07); // Bit 0-2
    SetMaxHarqReTxDelay(byte2 & 0x1F);       // Bit 3-7

    // Byte 3
    uint8_t byte3 = i.ReadU8();
    SetHarqProcessesRx((byte3 >> 5) & 0x07); // Bit 0-2
    SetMaxHarqReRxDelay(byte3 & 0x1F);       // Bit 3-7

    // Byte 4
    uint8_t byte4 = i.ReadU8();
    SetFlowId(byte4 & 0x3F); // Bit 2-7

    if (m_ftMode == 1)
    {
        // Byte 5
        uint8_t byte5 = i.ReadU8();
        SetNetworkBeaconPeriod(static_cast<NetworkBeaconPeriod>((byte5 >> 4) & 0x0F)); // Bit 0-3
        SetClusterBeaconPeriod(static_cast<ClusterBeaconPeriod>(byte5 & 0x0F));        // Bit 4-7

        // Byte 6-7
        uint8_t byte6 = i.ReadU8();
        SetNextClusterChannel(((byte6 & 0x1F) << 8) | (i.ReadU8() & 0xFF)); // Bit 3-7

        // Byte 8-11
        SetTimeToNext(i.ReadNtohU32());
    }

    if (m_current == 1)
    {
        // Byte 12-13
        uint8_t byte12 = i.ReadU8();
        SetCurrentClusterChannel(((byte12 & 0x1F) << 8) | (i.ReadU8() & 0xFF)); // Bit 3-7
    }

    return i.GetDistanceFrom(start);
}

void
Dect2020AssociationRequestMessage::SetSetupCause(uint8_t cause)
{
    m_setupCause = cause & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetSetupCause() const
{
    return m_setupCause;
}

void
Dect2020AssociationRequestMessage::SetNumberOfFlows(uint8_t flows)
{
    m_numberOfFlows = flows & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetNumberOfFlows() const
{
    return m_numberOfFlows;
}

void
Dect2020AssociationRequestMessage::SetPowerConstraints(bool constraints)
{
    m_powerConstraints = constraints;
}

bool
Dect2020AssociationRequestMessage::GetPowerConstraints() const
{
    return m_powerConstraints;
}

void
Dect2020AssociationRequestMessage::SetFtMode(bool mode)
{
    m_ftMode = mode;
}

bool
Dect2020AssociationRequestMessage::GetFtMode() const
{
    return m_ftMode;
}

void
Dect2020AssociationRequestMessage::SetCurrent(bool current)
{
    m_current = current;
}

bool
Dect2020AssociationRequestMessage::GetCurrent() const
{
    return m_current;
}

void
Dect2020AssociationRequestMessage::SetHarqProcessesTx(uint8_t value)
{
    m_harqProcessesTx = value & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetHarqProcessesTx() const
{
    return m_harqProcessesTx;
}

void
Dect2020AssociationRequestMessage::SetMaxHarqReTxDelay(uint8_t delay)
{
    m_maxHarqReTxDelay = delay & 0x1F; // 5 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetMaxHarqReTxDelay() const
{
    return m_maxHarqReTxDelay;
}

void
Dect2020AssociationRequestMessage::SetHarqProcessesRx(uint8_t value)
{
    m_harqProcessesRx = value & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetHarqProcessesRx() const
{
    return m_harqProcessesRx;
}

void
Dect2020AssociationRequestMessage::SetMaxHarqReRxDelay(uint8_t delay)
{
    m_maxHarqReRxDelay = delay & 0x1F; // 5 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetMaxHarqReRxDelay() const
{
    return m_maxHarqReRxDelay;
}

void
Dect2020AssociationRequestMessage::SetFlowId(uint8_t id)
{
    m_flowId = id & 0x3F; // 6 Bit
}

uint8_t
Dect2020AssociationRequestMessage::GetFlowId() const
{
    return m_flowId;
}

void
Dect2020AssociationRequestMessage::SetNetworkBeaconPeriod(NetworkBeaconPeriod value)
{
    m_networkBeaconPeriod = value;
}

NetworkBeaconPeriod
Dect2020AssociationRequestMessage::GetNetworkBeaconPeriod() const
{
    return m_networkBeaconPeriod;
}

void
Dect2020AssociationRequestMessage::SetClusterBeaconPeriod(ClusterBeaconPeriod value)
{
    m_clusterBeaconPeriod = value;
}

ClusterBeaconPeriod
Dect2020AssociationRequestMessage::GetClusterBeaconPeriod() const
{
    return m_clusterBeaconPeriod;
}

void
Dect2020AssociationRequestMessage::SetNextClusterChannel(uint16_t channel)
{
    m_nextClusterChannel = channel & 0x1FFF; // 13 Bit
}

uint16_t
Dect2020AssociationRequestMessage::GetNextClusterChannel() const
{
    return m_nextClusterChannel;
}

void
Dect2020AssociationRequestMessage::SetTimeToNext(uint32_t time)
{
    m_timeToNext = time;
}

uint32_t
Dect2020AssociationRequestMessage::GetTimeToNext() const
{
    return m_timeToNext;
}

void
Dect2020AssociationRequestMessage::SetCurrentClusterChannel(uint16_t channel)
{
    m_currentClusterChannel = channel & 0x1FFF; // 13 Bit
}

uint16_t
Dect2020AssociationRequestMessage::GetCurrentClusterChannel() const
{
    return m_currentClusterChannel;
}

// *******************************************************
//            DECT2020 Association Response Message
//            # ETSI TS 103 636-4 V2.1.1 6.4.2.5
// *******************************************************

Dect2020AssociationResponseMessage::Dect2020AssociationResponseMessage()
{
}

Dect2020AssociationResponseMessage::~Dect2020AssociationResponseMessage()
{
}

TypeId
Dect2020AssociationResponseMessage::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020AssociationResponseMessage")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020AssociationResponseMessage>();
    return tid;
}

TypeId
Dect2020AssociationResponseMessage::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
Dect2020AssociationResponseMessage::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;
    byte0 |= (m_associationAccepted << 7); // Bit 0
    // Bit 1 Reserved
    byte0 |= (m_harqMod << 5);                       // Bit 2
    byte0 |= (m_numberOfFlows & 0x07) << 2;          // Bit 3-5
    byte0 |= (m_groupIdAndResourceTagIncluded << 1); // Bit 6
    // Bit 7 Reserved

    start.WriteU8(byte0);

    if (m_associationAccepted == 0) // Association rejected
    {
        // Byte 1
        uint8_t byte1 = 0;
        byte1 |= (m_rejectCause & 0x0F) << 4; // Bit 0-3
        byte1 |= (m_rejectTimer & 0x0F);      // Bit 4-7

        start.WriteU8(byte1);

        NS_LOG_INFO("Serialize: byte1 = 0x" << std::hex << static_cast<uint32_t>(byte1));
    }

    if (m_harqMod == 1) // HARQ configuration present
    {
        // Byte 2
        uint8_t byte2 = 0;
        byte2 |= (m_harqProcessesRx & 0x07) << 5; // Bit 0-2
        byte2 |= m_maxHarqReRxDelay & 0x1F;       // Bit 3-7

        start.WriteU8(byte2);

        // Byte 3
        uint8_t byte3 = 0;
        byte3 |= (m_harqProcessesTx & 0x07) << 5; // Bit 0-2
        byte3 |= m_maxHarqReTxDelay & 0x1F;       // Bit 3-7

        start.WriteU8(byte3);
    }

    // Byte 4
    uint8_t byte4 = 0;
    byte4 |= (m_flowId & 0x3F); // Bit 2-7

    start.WriteU8(byte4);

    if (m_groupIdAndResourceTagIncluded == 1)
    {
        // Byte 5
        uint8_t byte5 = 0;
        byte5 |= (m_groupId & 0x7F); // Bit 1-7

        start.WriteU8(byte5);

        // Byte 6
        uint8_t byte6 = 0;
        byte6 |= (m_resourceTag & 0x7F); // Bit 1-7

        start.WriteU8(byte6);
    }
}

uint32_t
Dect2020AssociationResponseMessage::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    // Byte 0
    uint8_t byte0 = i.ReadU8();
    SetAssociationAccepted((byte0 >> 7) & 0x01); // Bit 0
    // Bit 1 Reserved
    SetHarqMod((byte0 >> 5) & 0x01);                       // Bit 2
    SetNumberOfFlows((byte0 >> 2) & 0x07);                 // Bit 3-5
    SetGroupIdAndResourceTagIncluded((byte0 >> 1) & 0x01); // Bit 6
    // Bit 7 Reserved

    if (m_associationAccepted == 0) // Association rejected
    {
        // Byte 1
        uint8_t byte1 = i.ReadU8();

        NS_LOG_INFO("Deserialize: byte1 = 0x" << std::hex << static_cast<uint32_t>(byte1));

        SetRejectCause((byte1 >> 4) & 0x0F); // Bit 0-3
        SetRejectTimer(byte1 & 0x0F);        // Bit 4-7
    }

    if (m_harqMod == 1) // HARQ configuration present
    {
        // Byte 2
        uint8_t byte2 = i.ReadU8();
        SetHarqProcessesRx((byte2 >> 5) & 0x07); // Bit 0-2
        SetMaxHarqReRxDelay(byte2 & 0x1F);       // Bit 3-7

        // Byte 3
        uint8_t byte3 = i.ReadU8();
        SetHarqProcessesTx((byte3 >> 5) & 0x07); // Bit 0-2
        SetMaxHarqReTxDelay(byte3 & 0x1F);       // Bit 3-7
    }

    // Byte 4
    uint8_t byte4 = i.ReadU8();
    SetFlowId(byte4 & 0x3F); // Bit 2-7

    if (m_groupIdAndResourceTagIncluded == 1)
    {
        // Byte 5
        uint8_t byte5 = i.ReadU8();
        SetGroupId(byte5 & 0x7F); // Bit 1-7

        // Byte 6
        uint8_t byte6 = i.ReadU8();
        SetResourceTag(byte6 & 0x7F); // Bit 1-7
    }

    return i.GetDistanceFrom(start);
}

void
Dect2020AssociationResponseMessage::Print(std::ostream& os) const
{
    os << "Dect2020AssociationResponseMessage:" << std::endl;
    os << "  Association Accepted: " << std::boolalpha << m_associationAccepted << std::endl;
    os << "  HARQ Mod: " << std::boolalpha << m_harqMod << std::endl;
    os << "  Number of Flows: " << static_cast<uint32_t>(m_numberOfFlows) << std::endl;
    os << "  GroupId and ResourceTag Included: " << std::boolalpha
       << m_groupIdAndResourceTagIncluded << std::endl;

    if (!m_associationAccepted)
    {
        os << "  Reject Cause: " << static_cast<uint32_t>(m_rejectCause) << std::endl;
        os << "  Reject Timer: " << static_cast<uint32_t>(m_rejectTimer) << std::endl;
    }

    if (m_harqMod)
    {
        os << "  HARQ Processes RX: " << static_cast<uint32_t>(m_harqProcessesRx) << std::endl;
        os << "  Max HARQ ReRX Delay: " << static_cast<uint32_t>(m_maxHarqReRxDelay) << std::endl;
        os << "  HARQ Processes TX: " << static_cast<uint32_t>(m_harqProcessesTx) << std::endl;
        os << "  Max HARQ ReTX Delay: " << static_cast<uint32_t>(m_maxHarqReTxDelay) << std::endl;
    }

    os << "  Flow ID: " << static_cast<uint32_t>(m_flowId) << std::endl;

    if (m_groupIdAndResourceTagIncluded)
    {
        os << "  Group ID: " << static_cast<uint32_t>(m_groupId) << std::endl;
        os << "  Resource Tag: " << static_cast<uint32_t>(m_resourceTag) << std::endl;
    }
}

uint32_t
Dect2020AssociationResponseMessage::GetSerializedSize() const
{
    uint32_t size = 1; // Byte 0

    if (!m_associationAccepted)
    {
        size += 1; // Byte 1: Reject Cause + Timer
    }

    if (m_harqMod)
    {
        size += 2; // Byte 2–3: HARQ RX + TX
    }

    size += 1; // Byte 4: Flow ID

    if (m_groupIdAndResourceTagIncluded)
    {
        size += 2; // Byte 5–6: Group ID + Resource Tag
    }

    return size;
}

void
Dect2020AssociationResponseMessage::SetAssociationAccepted(bool accepted)
{
    m_associationAccepted = accepted;
}

bool
Dect2020AssociationResponseMessage::GetAssociationAccepted() const
{
    return m_associationAccepted;
}

void
Dect2020AssociationResponseMessage::SetHarqMod(bool harqMod)
{
    m_harqMod = harqMod;
}

bool
Dect2020AssociationResponseMessage::GetHarqMod() const
{
    return m_harqMod;
}

void
Dect2020AssociationResponseMessage::SetNumberOfFlows(uint8_t numberOfFlows)
{
    m_numberOfFlows = numberOfFlows & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetNumberOfFlows() const
{
    return m_numberOfFlows;
}

void
Dect2020AssociationResponseMessage::SetGroupIdAndResourceTagIncluded(bool included)
{
    m_groupIdAndResourceTagIncluded = included;
}

bool
Dect2020AssociationResponseMessage::GetGroupIdAndResourceTagIncluded() const
{
    return m_groupIdAndResourceTagIncluded;
}

void
Dect2020AssociationResponseMessage::SetRejectCause(uint8_t cause)
{
    m_rejectCause = cause & 0x0F; // 4 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetRejectCause() const
{
    return m_rejectCause;
}

void
Dect2020AssociationResponseMessage::SetRejectTimer(uint8_t timer)
{
    m_rejectTimer = timer & 0x0F; // 4 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetRejectTimer() const
{
    return m_rejectTimer;
}

void
Dect2020AssociationResponseMessage::SetHarqProcessesRx(uint8_t value)
{
    m_harqProcessesRx = value & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetHarqProcessesRx() const
{
    return m_harqProcessesRx;
}

void
Dect2020AssociationResponseMessage::SetMaxHarqReRxDelay(uint8_t delay)
{
    m_maxHarqReRxDelay = delay & 0x1F; // 5 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetMaxHarqReRxDelay() const
{
    return m_maxHarqReRxDelay;
}

void
Dect2020AssociationResponseMessage::SetHarqProcessesTx(uint8_t value)
{
    m_harqProcessesTx = value & 0x07; // 3 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetHarqProcessesTx() const
{
    return m_harqProcessesTx;
}

void
Dect2020AssociationResponseMessage::SetMaxHarqReTxDelay(uint8_t delay)
{
    m_maxHarqReTxDelay = delay & 0x1F; // 5 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetMaxHarqReTxDelay() const
{
    return m_maxHarqReTxDelay;
}

void
Dect2020AssociationResponseMessage::SetFlowId(uint8_t flowId)
{
    m_flowId = flowId & 0x3F; // 6 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetFlowId() const
{
    return m_flowId;
}

void
Dect2020AssociationResponseMessage::SetGroupId(uint8_t groupId)
{
    m_groupId = groupId & 0x7F; // 7 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetGroupId() const
{
    return m_groupId;
}

void
Dect2020AssociationResponseMessage::SetResourceTag(uint8_t resourceTag)
{
    m_resourceTag = resourceTag & 0x7F; // 7 Bit
}

uint8_t
Dect2020AssociationResponseMessage::GetResourceTag() const
{
    return m_resourceTag;
}

} // namespace ns3