#ifndef DECT2020_OPERATING_BAND_H
#define DECT2020_OPERATING_BAND_H

#include <ns3/core-module.h>
#include "ns3/type-id.h"
#include "ns3/object.h"

#include <cstdint>
#include <vector>

namespace ns3
{

enum OperatingChannelBandwith
{
    BANDWITH_I = 1539000 // Transmission Bandwith #ETSI 103 636-2 V1.5.1 Table 5.3.2.1
    // TBD: BANDWITH II & BANDWITH III
};

struct ChannelInfo
{
    uint16_t channelNumber;
    double centerFrequency;
    double rssi;
};

struct BandParameters // #ETSI 103 636-2 V1.5.1 Chapter 5.3
{
  public:
    uint8_t bandNumber;    // Band 1-22
    double startFrequency; // Absolute start channel centre frequency #ETSI 103 636-2 V1.5.1
                           // Table 5.4.2-1
    double stopFrequency;  // Absolute stop channel centre frequency #ETSI 103 636-2 V1.5.1
                           // Table 5.4.2-1
    double frequencyStep;
    uint16_t nStart;
    uint16_t nEnd;
};

class Dect2020OperatingBand : public Object
{
  public:
    static TypeId GetTypeId(void);
    Dect2020OperatingBand(uint8_t bandNumber, OperatingChannelBandwith bandwidth);

    std::vector<ChannelInfo> GetAvailableChannels() const;
    void UpdateChannelRssi(uint16_t channelNumber, double rssi);

    BandParameters m_bandParameters;
    uint8_t m_bandNumber;
    OperatingChannelBandwith m_operatingChannelBandwidth;
  private:
    void InitializeBand();
    BandParameters InitializeBandParameters(uint8_t bandNumber, OperatingChannelBandwith bandwith);
};
} // namespace ns3
#endif