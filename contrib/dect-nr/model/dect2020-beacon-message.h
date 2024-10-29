#ifndef DECT2020_BEACON_MESSAGE_H
#define DECT2020_BEACON_MESSAGE_H

#include "ns3/header.h"
#include "ns3/log.h"

#include <vector>

NS_LOG_COMPONENT_DEFINE("Dect2020BeaconMessage");

namespace ns3
{

class Dect2020BeaconMessage : public Header
{
  public:
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

    void SetNetworkBeaconPeriod(uint8_t networkBeaconPeriod);
    uint8_t GetNetworkBeaconPeriod() const;

    void SetClusterBeaconPeriod(uint8_t clusterBeaconPeriod);
    uint8_t GetClusterBeaconPeriod() const;

    void SetNextClusterChannel(uint16_t nextClusterChannel);
    uint16_t GetNextClusterChannel() const;

    void SetTimeToNext(uint32_t timeToNext);
    uint32_t GetTimeToNext() const;

    void SetClustersMaxTxPower(uint8_t clustersMaxTxPower);
    uint8_t GetClustersMaxTxPower() const;

    void SetCurrentClusterChannel(uint16_t currentClusterChannel);
    uint16_t GetCurrentClusterChannel() const;

    void SetAdditionalNetworkBeaconChannels(uint16_t additionalNetworkBeaconChannels);
    uint16_t GetAdditionalNetworkBeaconChannels() const;

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
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
    uint8_t m_networkBeaconPeriod;
    uint8_t m_clusterBeaconPeriod;
    uint16_t m_nextClusterChannel;
    uint32_t m_timeToNext;
    uint8_t m_clustersMaxTxPower;
    uint16_t m_currentClusterChannel;
    uint16_t m_additionalNetworkBeaconChannels;
}

} // namespace ns3

#endif