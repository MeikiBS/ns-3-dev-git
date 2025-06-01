#ifndef DECT2020_CHANNEL_MANAGER_H
#define DECT2020_CHANNEL_MANAGER_H

#include "ns3/dect2020-channel.h"

#include <cstdint>
#include <vector>
/**
 * @file dect2020-channel-manager.h
 * @brief Channel manager for DECT-2020 NR bands and logical channel IDs.
 *
 * This file defines the Dect2020ChannelManager class, which handles the management of logical
 * DECT channels, their spectrum models, RSSI monitoring, and power tracking. It is based on the
 * ETSI TS 103 636-2 specification and is used to abstract band-specific frequency planning,
 * channel ID mapping, and signal power tracking for simulation purposes.
 */

namespace ns3
{
// Currently not used
enum OperatingChannelBandwith
{
    BANDWITH_I = 1728000 // Nominal Channel Bandwith #ETSI 103 636-2 V1.5.1 Table 5.3.2.1
    // TBD: BANDWITH II & BANDWITH III
};


/**
 * \struct BandParameters
 * \brief Describes parameters for a DECT band.
 *
 * This structure defines the properties of a DECT band as specified in
 * ETSI TS 103 636-2 V1.5.1, Table 5.4.2-1. It includes the frequency range and 
 * step size used to compute valid logical channel numbers.
 */
struct BandParameters
{
public:
  uint8_t bandNumber;     ///< DECT band number (1–22)
  double startFrequency;  ///< Center frequency of first channel in the band [Hz]
  double stopFrequency;   ///< Center frequency of last channel in the band [Hz]
  double frequencyStep;   ///< Channel raster (spacing) within the band [Hz]
  uint16_t nStart;        ///< Logical channel number for start frequency
  uint16_t nEnd;          ///< Logical channel number for stop frequency
};

/**
 * @class Dect2020ChannelManager
 * @brief Centralized channel management class for DECT-2020 NR simulations.
 *
 * This class is responsible for:
 * - Initializing DECT-2020 NR bands and their logical channels
 * - Providing access to valid channels based on the selected band
 * - Mapping between channel numbers and center frequencies
 * - Maintaining spectrum models and power usage for each band
 * - Supporting RSSI calculations and channel occupancy tracking
 *
 * Each DECT band (1–22) is defined by parameters such as start/stop frequency and channel raster.
 * These are used to compute logical channels and their corresponding SpectrumModel.
 *
 * Key ETSI reference: ETSI TS 103 636-2 V1.5.1, Table 5.4.2-1 and 5.3.2.1.
 */
class Dect2020ChannelManager
{
  public:
    Dect2020ChannelManager();
    ~Dect2020ChannelManager();

    // ### Channel Management
    void InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor);
    static std::vector<Ptr<Dect2020Channel>> GetValidChannels(uint8_t bandNumber);
    void UpdateChannelRssi(uint16_t channelNumber, double rssi);
    static BandParameters GetBandParameters(uint8_t bandNumber);
    static double CalculateCenterFrequency(uint8_t bandNumber, uint32_t channelNumber);
    static uint16_t GetFirstValidChannelNumber(uint8_t bandNumber);
    static uint8_t GetBandNumber(uint16_t channelNumber);
    static bool ChannelExists(uint32_t chId);
    static uint16_t GetChannelId(double centerFrequency);

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
    static std::map<uint16_t, std::vector<Ptr<Dect2020Channel>>> m_channels;  // Band :: Channels

  private:
    void InitializeBand();
    void CalculateChannelFrequencies();

    void InitializeChannels(uint8_t bandNumber);

    std::vector<Ptr<Dect2020Channel>> m_channelList;
    static std::map<uint8_t, Ptr<SpectrumModel>> m_bandModels;  // Band ID  : SpectrumModel
};

} // namespace ns3

#endif