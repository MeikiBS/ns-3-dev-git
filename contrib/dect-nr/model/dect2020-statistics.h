#include <stdint.h>

class Dect2020Statistics
{
  public:
    static void IncrementClusterBeaconReception();
    static uint32_t GetClusterBeaconReceptionCount();

  private:
    static uint32_t m_clusterBeaconReceptionCount;
};
