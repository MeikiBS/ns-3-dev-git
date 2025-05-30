#include "dect2020-statistics.h"

uint32_t Dect2020Statistics::m_clusterBeaconReceptionCount = 0;

void
Dect2020Statistics::IncrementClusterBeaconReception()
{
    ++m_clusterBeaconReceptionCount;
}

uint32_t
Dect2020Statistics::GetClusterBeaconReceptionCount()
{
    return m_clusterBeaconReceptionCount;
}

uint32_t Dect2020Statistics::m_clusterBeaconTransmissionCount = 0;

void
Dect2020Statistics::IncrementClusterBeaconTransmission()
{
    ++m_clusterBeaconTransmissionCount;
}

uint32_t
Dect2020Statistics::GetClusterBeaconTransmissionCount()
{
    return m_clusterBeaconTransmissionCount;
}

uint32_t Dect2020Statistics::m_packetsDroppedLowRssi = 0;

void
Dect2020Statistics::IncrementPacketsDroppedLowRssi()
{
    ++m_packetsDroppedLowRssi;
}

uint32_t
Dect2020Statistics::GetPacketsDroppedLowRssiCount()
{
    return m_packetsDroppedLowRssi;
}

uint32_t Dect2020Statistics::m_sumOfAllPacketsSent = 0;

void
Dect2020Statistics::IncrementSumOfAllPacketsSent()
{
    ++m_sumOfAllPacketsSent;
}

uint32_t
Dect2020Statistics::GetSumOfAllPacketsSent()
{
    return m_sumOfAllPacketsSent;
}
