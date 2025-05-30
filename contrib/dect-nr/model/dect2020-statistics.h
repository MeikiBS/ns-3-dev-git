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

    static void IncrementSumOfAllPacketsSent();
    static uint32_t GetSumOfAllPacketsSent();

  private:
    static uint32_t m_clusterBeaconReceptionCount;
    static uint32_t m_clusterBeaconTransmissionCount;
    static uint32_t m_packetsDroppedLowRssi;
    static uint32_t m_sumOfAllPacketsSent;
};
