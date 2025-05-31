#ifndef DECT2020_STATISTICS_H
#define DECT2020_STATISTICS_H

#include <stdint.h>

class Dect2020Statistics
{
public:
    static void IncrementClusterBeaconReception();
    static uint32_t GetClusterBeaconReceptionCount();

    static void IncrementClusterBeaconTransmission();
    static uint32_t GetClusterBeaconTransmissionCount();

    static void IncrementPacketsDroppedLowRssi();
    static uint32_t GetPacketsDroppedLowRssiCount();

    static void IncrementPacketsDroppedCollision();
    static uint32_t GetPacketsDroppedCollisionCount();

    static void IncrementSumOfAllPacketsSent();
    static uint32_t GetSumOfAllPacketsSent();


    static void IncrementNetworkBeaconReceptionCount();
    static uint32_t GetNetworkBeaconReceptionCount();

    static void IncrementNetworkBeaconTransmissionCount();
    static uint32_t GetNetworkBeaconTransmissionCount();

    static void IncrementAssociationRequestTransmissionCount();
    static uint32_t GetAssociationRequestTransmissionCount();

    static void IncrementAssociationRequestReceptionCount();
    static uint32_t GetAssociationRequestReceptionCount();

    static void IncrementAssociationResponseTransmissionCount();
    static uint32_t GetAssociationResponseTransmissionCount();

    static void IncrementAssociationResponseReceptionCount();
    static uint32_t GetAssociationResponseReceptionCount();

private:
    static uint32_t m_clusterBeaconReceptionCount;
    static uint32_t m_clusterBeaconTransmissionCount;

    static uint32_t m_packetsDroppedLowRssi;
    static uint32_t m_packetsDroppedCollision;
    static uint32_t m_sumOfAllPacketsSent;

    static uint32_t m_networkBeaconReceptionCount;
    static uint32_t m_networkBeaconTransmissionCount;

    static uint32_t m_associationRequestReceptionCount;
    static uint32_t m_associationRequestTransmissionCount;

    static uint32_t m_associationResponseReceptionCount;
    static uint32_t m_associationResponseTransmissionCount;
};

#endif // DECT2020_STATISTICS_H
