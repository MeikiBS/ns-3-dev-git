#ifndef DECT2020_CHANNEL_MANAGER_H
#define DECT2020_CHANNEL_MANAGER_H

#include "ns3/dect2020-channel.h"

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

class Dect2020ChannelManager
{
  public:
    Dect2020ChannelManager();
    ~Dect2020ChannelManager();

    void InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor);
    static std::vector<Ptr<Dect2020Channel>> GetValidChannels(uint8_t bandNumber);

    std::vector<ChannelInfo> GetAvailableChannels() const;
    void UpdateChannelRssi(uint16_t channelNumber, double rssi);
    static BandParameters GetBandParameters(uint8_t bandNumber);
    static double CalculateCenterFrequency(uint8_t bandNumber, uint32_t channelNumber);
    static uint16_t GetFirstValidChannelNumber(uint8_t bandNumber);
    static uint8_t GetBandNumber(uint16_t channelNumber);

    BandParameters m_bandParameters;
    uint8_t m_bandNumber;
    OperatingChannelBandwith m_operatingChannelBandwidth;

    // #### Spectrum Model Manager
    static Ptr<SpectrumModel> GetSpectrumModel(uint8_t bandId);
    static void AddSpectrumPowerToChannel(uint16_t channelId, double powerWatt);
    static void RemoveSpectrumPowerFromChannel(uint16_t channelId, double powerWatt);
    static double GetRssiDbm(uint16_t channelId);
    static double DbmToW(double dBm);
    static double WToDbm(double w);

    static std::map<uint8_t, Ptr<SpectrumValue>> m_channelOccupancy;
    static std::map<uint16_t, std::vector<Ptr<Dect2020Channel>>> m_channels;

  private:
    void InitializeBand();
    void CalculateChannelFrequencies();

    void InitializeChannels(uint8_t bandNumber);

    std::vector<Ptr<Dect2020Channel>> m_channelList;
    static std::map<uint8_t, Ptr<SpectrumModel>> m_bandModels;  // Band ID  : SpectrumModel
};

} // namespace ns3

#endif