#ifndef DECT2020_MAC_MESSAGES_H
#define DECT2020_MAC_MESSAGES_H

#include "ns3/header.h"
#include "ns3/log.h"

#include <vector>

namespace ns3
{

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

// *******************************************************
//            DECT2020 Cluster Beacon Message
//            # ETSI TS 103 636-4 V2.1.1 6.4.2.3
// *******************************************************

class Dect2020ClusterBeaconMessage : public Header
{
  public:
    Dect2020ClusterBeaconMessage();
    virtual ~Dect2020ClusterBeaconMessage();

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

    int8_t GetTxPowerFromField(uint8_t field);

    void SetSystemFrameNumber(uint8_t sfn);
    uint8_t GetSystemFrameNumber() const;

    void SetTxPowerIncluded(bool txPowerIncluded);
    bool GetTxPowerIncluded() const;

    void SetPowerConstraints(bool powerConstraints);
    bool GetPowerConstraints() const;

    void SetFrameOffsetIncluded(bool frameOffsetIncluded);
    bool GetFrameOffsetIncluded() const;

    void SetNextChannelIncluded(bool nextChannelIncluded);
    bool GetNextChannelIncluded() const;

    void SetTimeToNextFieldPresent(bool timeToNextFieldPresent);
    bool GetTimeToNextFieldPresent() const;

    void SetNetworkBeaconPeriod(NetworkBeaconPeriod period);
    NetworkBeaconPeriod GetNetworkBeaconPeriod() const;

    void SetClusterBeaconPeriod(ClusterBeaconPeriod period);
    ClusterBeaconPeriod GetClusterBeaconPeriod() const;

    void SetCountToTrigger(uint8_t count);
    uint8_t GetCountToTrigger() const;

    void SetRelativeQuality(uint8_t relQuality);
    uint8_t GetRelativeQuality() const;

    void SetMinimumQuality(uint8_t minQuality);
    uint8_t GetMinimumQuality() const;

    void SetClusterMaxTxPower(uint8_t power);
    uint8_t GetClusterMaxTxPower() const;

    void SetFrameOffset(uint16_t offset);
    uint16_t GetFrameOffset() const;

    void SetNextClusterChannel(uint16_t channel);
    uint16_t GetNextClusterChannel() const;

    void SetTimeToNext(uint32_t time);
    uint32_t GetTimeToNext() const;

  private:
    uint8_t m_SFN; // System Frame number
    bool m_txPowerIncluded;
    bool m_powerConstraints;
    bool m_FO; // Frame Offset
    bool m_nextChannel;
    bool m_timeToNextFieldPresent; // Time to Next Field: 0 = not present, 1 = present
    NetworkBeaconPeriod m_networkBeaconPeriod;
    ClusterBeaconPeriod m_clusterBeaconPeriod;
    uint8_t m_countToTrigger;
    uint8_t m_relQuality;
    uint8_t m_minQuality;
    uint8_t m_clusterMaxTxPower;
    uint16_t
        m_frameOffset; // if subcarrierscaling factor = <= 4 --> frameOffset 8 bit, otherwise 16 bit
    uint16_t m_nextClusterChannel;
    uint32_t m_timeToNext;
};

// *******************************************************
//            DECT2020 Network Beacon Message
//            # ETSI TS 103 636-4 V2.1.1 6.4.2.2
// *******************************************************

class Dect2020NetworkBeaconMessage : public Header
{
  public:
    Dect2020NetworkBeaconMessage();
    virtual ~Dect2020NetworkBeaconMessage();

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

// *******************************************************
//            DECT2020 Association Request Message
//            # ETSI TS 103 636-4 V2.1.1 6.4.2.4
// *******************************************************

class Dect2020AssociationRequestMessage : public Header
{
  public:
    Dect2020AssociationRequestMessage();
    virtual ~Dect2020AssociationRequestMessage();

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

    // Getter and Setter
    void SetSetupCause(uint8_t cause);
    uint8_t GetSetupCause() const;

    void SetNumberOfFlows(uint8_t flows);
    uint8_t GetNumberOfFlows() const;

    void SetPowerConstraints(bool constraints);
    bool GetPowerConstraints() const;

    void SetFtMode(bool mode);
    bool GetFtMode() const;

    void SetCurrent(bool current);
    bool GetCurrent() const;

    void SetHarqProcessesTx(uint8_t value);
    uint8_t GetHarqProcessesTx() const;

    void SetMaxHarqReTxDelay(uint8_t delay);
    uint8_t GetMaxHarqReTxDelay() const;

    void SetHarqProcessesRx(uint8_t value);
    uint8_t GetHarqProcessesRx() const;

    void SetMaxHarqReRxDelay(uint8_t delay);
    uint8_t GetMaxHarqReRxDelay() const;

    void SetFlowId(uint8_t id);
    uint8_t GetFlowId() const;

    void SetNetworkBeaconPeriod(NetworkBeaconPeriod value);
    NetworkBeaconPeriod GetNetworkBeaconPeriod() const;

    void SetClusterBeaconPeriod(ClusterBeaconPeriod value);
    ClusterBeaconPeriod GetClusterBeaconPeriod() const;

    void SetNextClusterChannel(uint16_t channel);
    uint16_t GetNextClusterChannel() const;

    void SetTimeToNext(uint32_t time);
    uint32_t GetTimeToNext() const;

    void SetCurrentClusterChannel(uint16_t channel);
    uint16_t GetCurrentClusterChannel() const;

  private:
    uint8_t m_setupCause;    // 3 Bit
    uint8_t m_numberOfFlows; // 3 Bit
    bool m_powerConstraints; // 1 Bit
    bool m_ftMode;           // 1 Bit
    bool m_current; // 1 Bit. 0 = current cluster channel is the same as the next cluster channel.
                    //        1 = not the same --> current cluster channel field included
    uint8_t m_harqProcessesTx;                 // 3 Bit
    uint8_t m_maxHarqReTxDelay;                // 5 Bit
    uint8_t m_harqProcessesRx;                 // 3 Bit
    uint8_t m_maxHarqReRxDelay;                // 5 Bit
    uint8_t m_flowId;                          // 6 Bit
    NetworkBeaconPeriod m_networkBeaconPeriod; // 4 Bit
    ClusterBeaconPeriod m_clusterBeaconPeriod; // 4 Bit
    uint16_t m_nextClusterChannel;             // 13 Bit
    uint32_t m_timeToNext;                     // 32 Bit
    uint16_t m_currentClusterChannel;          // 13 Bit
};

// *******************************************************
//            DECT2020 Association Response Message
//            # ETSI TS 103 636-4 V2.1.1 6.4.2.5
// *******************************************************

class Dect2020AssociationResponseMessage : public Header
{
  public:
    Dect2020AssociationResponseMessage();
    virtual ~Dect2020AssociationResponseMessage();

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

    // Getter and Setter
    void SetAssociationAccepted(bool accepted);
    bool GetAssociationAccepted() const;

    void SetHarqMod(bool harqMod);
    bool GetHarqMod() const;

    void SetNumberOfFlows(uint8_t numberOfFlows);
    uint8_t GetNumberOfFlows() const;

    void SetGroupIdAndResourceTagIncluded(bool included);
    bool GetGroupIdAndResourceTagIncluded() const;

    void SetRejectCause(uint8_t cause);
    uint8_t GetRejectCause() const;

    void SetRejectTimer(uint8_t timer);
    uint8_t GetRejectTimer() const;

    void SetHarqProcessesRx(uint8_t value);
    uint8_t GetHarqProcessesRx() const;

    void SetMaxHarqReRxDelay(uint8_t delay);
    uint8_t GetMaxHarqReRxDelay() const;

    void SetHarqProcessesTx(uint8_t value);
    uint8_t GetHarqProcessesTx() const;

    void SetMaxHarqReTxDelay(uint8_t delay);
    uint8_t GetMaxHarqReTxDelay() const;

    void SetFlowId(uint8_t flowId);
    uint8_t GetFlowId() const;

    void SetGroupId(uint8_t groupId);
    uint8_t GetGroupId() const;

    void SetResourceTag(uint8_t resourceTag);
    uint8_t GetResourceTag() const;

  private:
    bool m_associationAccepted;           // 1 Bit
    bool m_harqMod;                       // 1 Bit
    uint8_t m_numberOfFlows;              // 3 Bit
    bool m_groupIdAndResourceTagIncluded; // 1 Bit
    uint8_t m_rejectCause;                // 4 Bit
    uint8_t m_rejectTimer;                // 4 Bit
    uint8_t m_harqProcessesRx;            // 3 Bit
    uint8_t m_maxHarqReRxDelay;           // 5 Bot
    uint8_t m_harqProcessesTx;            // 3 Bit
    uint8_t m_maxHarqReTxDelay;           // 5 Bit
    uint8_t m_flowId;                     // 6 Bit
    uint8_t m_groupId;                    // 7 Bit
    uint8_t m_resourceTag;                // 7 Bit
};

} // namespace ns3

#endif