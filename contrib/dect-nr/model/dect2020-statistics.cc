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
