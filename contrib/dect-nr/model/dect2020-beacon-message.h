#ifndef DECT2020_BEACON_MESSAGE_H
#define DECT2020_BEACON_MESSAGE_H

#include "ns3/header.h"
#include "ns3/log.h"

#include <vector>

namespace ns3
{

class Dect2020BeaconMessage : public Header
{
  public:
    enum NetworkBeaconPeriod // ETSI TS 103 636-4 V 1.51 #6.4.2.2-1
    {
        NETWORK_PERIOD_50MS = 0,
        NETWORK_PERIOD_100MS = 1,
        NETWORK_PERIOD_500MS = 2,
        NETWORK_PERIOD_1000MS = 3,
        NETWORK_PERIOD_1500MS = 4,
        NETWORK_PERIOD_2000MS = 5,
        NETWORK_PERIOD_4000MS = 6
    };

    enum ClusterBeaconPeriod // ETSI TS 103 636-4 V 1.51 #6.4.2.2-1
    {
        CLUSTER_PERIOD_10MS = 0,
        CLUSTER_PERIOD_50MS = 1,
        CLUSTER_PERIOD_100MS = 2,
        CLUSTER_PERIOD_500MS = 3,
        CLUSTER_PERIOD_1000MS = 4,
        CLUSTER_PERIOD_1500MS = 5,
        CLUSTER_PERIOD_2000MS = 6,
        CLUSTER_PERIOD_4000MS = 7,
        CLUSTER_PERIOD_8000MS = 8,
        CLUSTER_PERIOD_16000MS = 9,
        CLUSTER_PERIOD_32000MS = 10
    };

    Dect2020BeaconMessage();
    virtual ~Dect2020BeaconMessage();

    // Setter und Getter für jedes Feld
    void SetTxPowerIncluded(bool txPowerIncluded);
    bool GetTxPowerIncluded() const;

    void SetPowerConstraints(bool powerConstraints);
    bool GetPowerConstraints() const;

    void SetCurrentClusterChannelIncluded(bool currentClusterChannelIncluded);
    bool GetCurrentClusterChannelIncluded() const;

    void SetNetworkBeaconChannels(uint8_t networkBeaconChannels);
    uint8_t GetNetworkBeaconChannels() const;

    void SetNetworkBeaconPeriod(NetworkBeaconPeriod networkBeaconPeriod);
    NetworkBeaconPeriod GetNetworkBeaconPeriod() const;

    void SetClusterBeaconPeriod(ClusterBeaconPeriod clusterBeaconPeriod);
    ClusterBeaconPeriod GetClusterBeaconPeriod() const;

    void SetNextClusterChannel(uint16_t nextClusterChannel);
    uint16_t GetNextClusterChannel() const;

    void SetTimeToNext(uint32_t timeToNext);
    uint32_t GetTimeToNext() const;

    void SetClustersMaxTxPower(uint8_t clustersMaxTxPower);
    uint8_t GetClustersMaxTxPower() const;

    void SetCurrentClusterChannel(uint16_t currentClusterChannel);
    uint16_t GetCurrentClusterChannel() const;

    void SetAdditionalNetworkBeaconChannels(uint16_t additionalNetworkBeaconChannels[3]);
    uint16_t* GetAdditionalNetworkBeaconChannels();

    uint16_t GetNetworkBeaconPeriodTime() const;

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

  private:
    // ETSI TS 103 636-4 V 1.51 #6.4.2.2-1
    bool m_txPowerIncluded;
    bool m_powerConstraints;
    bool m_currentClusterChannelIncluded;
    uint8_t m_networkBeaconChannels;
    NetworkBeaconPeriod m_networkBeaconPeriod;
    ClusterBeaconPeriod m_clusterBeaconPeriod;
    uint16_t m_nextClusterChannel;
    uint32_t m_timeToNext;
    uint8_t m_clustersMaxTxPower;
    uint16_t m_currentClusterChannel;
    uint16_t m_additionalNetworkBeaconChannels[3]{};
};

} // namespace ns3

#endif