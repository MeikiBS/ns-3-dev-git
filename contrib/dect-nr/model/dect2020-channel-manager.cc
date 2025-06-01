#include "dect2020-channel-manager.h"

#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("Dect2020ChannelManager");

namespace ns3
{

std::map<uint16_t, std::vector<Ptr<Dect2020Channel>>> Dect2020ChannelManager::m_channels;

Dect2020ChannelManager::Dect2020ChannelManager()
{
}

Dect2020ChannelManager::~Dect2020ChannelManager()
{
}

/**
 * \brief Initializes all DECT-2020 NR channels for a given band and scaling factor.
 *
 * Creates logical DECT channels using the band configuration and populates each channel
 * with time-domain slots and subslots based on the subcarrier scaling factor.
 *
 * - The number of slots per frame is fixed to 24 (ETSI TS 103 636-3 V1.5.1, Section 4.4).
 * - The subslot count per slot depends on the subcarrier scaling factor {1, 2, 4, 8}.
 * - Each channel is assigned a center frequency using \ref CalculateCenterFrequency.
 *
 * \param bandNumber DECT band index (1–22), see ETSI TS 103 636-2 Table 5.4.2-1
 * \param subcarrierScalingFactor Scaling factor used to derive subslots per slot (1, 2, 4, 8)
 */
void
Dect2020ChannelManager::InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor)
{
    const uint32_t slotsPerFrame = 24; // #ETSI 103 636-3 V1.5.1, Section 4.4

    BandParameters bandParams = GetBandParameters(bandNumber);
    uint32_t numChannels = (bandParams.nEnd - bandParams.nStart) + 1; // Number of Channels

    if (!(subcarrierScalingFactor == 1 || subcarrierScalingFactor == 2 ||
          subcarrierScalingFactor == 4 || subcarrierScalingFactor == 8))
    {
        NS_LOG_INFO("Subcarrier scaling factor invalid, set to 1");
        subcarrierScalingFactor = 1;
    }

    // Compute the subslot duration in ns
    uint32_t numSubslotsPerSlot = (subcarrierScalingFactor == 1)   ? 2
                                  : (subcarrierScalingFactor == 2) ? 4
                                  : (subcarrierScalingFactor == 4) ? 8
                                                                   : 16;
    const double slotDurationNs = 10000000 / slotsPerFrame;         // 0,41667 ms in ns
    double subslotDurationNs = slotDurationNs / numSubslotsPerSlot; // Subslot Duration in ns

    // Initialize the logical channels
    this->m_channels.clear();
    for (uint32_t ch = 0; ch < numChannels; ch++)
    {
        int channelFrequencyNumbering = bandParams.nStart + ch;

        Ptr<Dect2020Channel> dect2020Channel = Create<Dect2020Channel>();
        dect2020Channel->m_channelId = channelFrequencyNumbering;
        dect2020Channel->m_centerFrequency =
            CalculateCenterFrequency(bandNumber, channelFrequencyNumbering);

        for (uint32_t slot = 0; slot < slotsPerFrame; slot++)
        {
            Slot slotObj;
            slotObj.slotId = slot;

            for (uint32_t ss = 0; ss < numSubslotsPerSlot; ss++)
            {
                Subslot subslot;
                subslot.subslotId = ss;
                subslot.status = SubslotStatus::FREE;
                subslot.rssi = 0.0;
                subslot.subslotDurationNs = subslotDurationNs;
                slotObj.subslots.push_back(subslot);
            }

            dect2020Channel->AddSlot(slotObj); // Add the current Slot to the Channel
        }

        m_channels[bandNumber].push_back(dect2020Channel);
    }
}

/**
 * \brief Returns the list of initialized DECT channels for the given band.
 *
 * Retrieves all Dect2020Channel objects that were initialized for the specified band.
 * If no channels were initialized for the band, an empty list is returned.
 *
 * A channel is considered "valid" if it fits entirely within the frequency range
 * of the selected DECT band and does not overlap adjacent channels beyond the band's bounds.
 *
 * \param bandNumber The DECT band index (1–22).
 * \return A vector of pointers to the corresponding Dect2020Channel objects.
 */
std::vector<Ptr<Dect2020Channel>>
Dect2020ChannelManager::GetValidChannels(uint8_t bandNumber)
{
    std::vector<Ptr<Dect2020Channel>> validChannels;

    auto it = m_channels.find(bandNumber);
    if (it != m_channels.end())
    {
        validChannels = it->second;
    }

    return validChannels;
}

/**
 * \brief Returns the frequency parameters for a specific DECT band.
 *
 * Looks up the band configuration for the given DECT band number, including start/stop
 * center frequencies, channel spacing (frequency step), and logical channel range.
 *
 * Band parameters are based on ETSI TS 103 636-2 V1.5.1, Table 5.4.2-1.
 *
 * \note Currently implemented bands: 1, 2, 20, and 21.
 * \throws Program aborts via NS_FATAL_ERROR if an unsupported band is provided.
 *
 * \param bandNumber The DECT band number (e.g., 1, 2, 20, 21).
 * \return A BandParameters struct containing frequency and channel information.
 */
BandParameters
Dect2020ChannelManager::GetBandParameters(uint8_t bandNumber)
{
    // Note: Currently only Bands 1, 2, 20, and 21 are implemented.
    BandParameters bp;
    bp.bandNumber = bandNumber;

    switch (bandNumber)
    {
    case 1:
        bp.startFrequency = 1881.792e6;
        bp.stopFrequency = 1899.072e6;
        bp.frequencyStep = 0.864e6;
        bp.nStart = 1657;
        bp.nEnd = 1677;

        break;

    case 2:
        bp.startFrequency = 1901.664e6;
        bp.stopFrequency = 1918.994e6;
        bp.frequencyStep = 0.864e6;
        bp.nStart = 1680;
        bp.nEnd = 1700;

        break;

    case 20:
        bp.startFrequency = 3802.388e6;
        bp.stopFrequency = 4198.1e6;

        bp.frequencyStep = 1.728e6;

        bp.nStart = 3416;
        bp.nEnd = 3645;

        break;

    case 21:
        bp.startFrequency = 3700.436e6;
        bp.stopFrequency = 3798.932e6;

        bp.frequencyStep = 1.728e6;

        bp.nStart = 3357;
        bp.nEnd = 3414;

        break;

    default:
        NS_FATAL_ERROR("Invalid band number. Supported bands are 1, 2, 20, and 21.");
    }

    return bp;
}

/**
 * \brief Returns the logical channel ID for a given center frequency.
 *
 * Iterates over all initialized channels and finds the channel whose center frequency
 * exactly matches the given value.
 *
 * \note This comparison uses exact floating-point equality. If your simulation setup
 *       introduces rounding errors, consider using a tolerance-based comparison.
 *
 * \param centerFrequency The center frequency in Hz.
 * \return The logical DECT channel ID, or 0 if no matching channel is found.
 */
uint16_t
Dect2020ChannelManager::GetChannelId(double centerFrequency)
{
    for (const auto& bandEntry : m_channels)
    {
        const std::vector<Ptr<Dect2020Channel>>& channelList = bandEntry.second;

        for (const auto& channel : channelList)
        {
            if (channel->m_centerFrequency == centerFrequency)
            {
                return channel->m_channelId;
            }
        }
    }

    NS_LOG_WARN("No channel found for center frequency " << centerFrequency);
    return 0;
}

/**
 * \brief Calculates the center frequency of a logical channel based on band and channel number.
 *
 * Uses the band-specific formula defined in ETSI TS 103 636-2 V1.5.1, Section 5.4.2 to compute
 * the absolute center frequency in Hz for a given DECT channel.
 *
 * Currently, only bands 1–12 and band 22 are implemented. Other bands require additional formulas.
 *
 * \param bandNumber The DECT band number (e.g., 1–22).
 * \param channelNumber The logical channel number (e.g., 1657 for Band 1).
 * \return The center frequency in Hz.
 */
double
Dect2020ChannelManager::CalculateCenterFrequency(uint8_t bandNumber, uint32_t channelNumber)
{
    double centerFrequency = 0;

    if ((bandNumber >= 1 && bandNumber <= 12) || bandNumber == 22)
    {
        double f0 = 450.144e6; // F0 in Hz
        double channelRaster = 0.864e6;

        centerFrequency = f0 + (channelNumber * channelRaster);
    }
    // TODO: Add more Bands

    return centerFrequency;
}

/**
 * \brief Checks whether a given channel ID exists in the initialized channel list.
 *
 * Determines if a channel with the specified ID has been created for its associated band.
 *
 * \param chId The logical DECT channel number to check.
 * \return True if the channel exists, false otherwise.
 */
bool
Dect2020ChannelManager::ChannelExists(uint32_t chId)
{
    std::vector<Ptr<Dect2020Channel>> validChannels = GetValidChannels(GetBandNumber(chId));

    for (const auto& ch : validChannels)
    {
        if (ch->m_channelId == chId)
        {
            return true;
        }
    }
    return false;
}

/**
 * \brief Returns the first valid channel number for the specified band.
 *
 * Based on ETSI TS 103 636-2 V1.5.1, Table 5.4.2-1.
 * Only supports bands 1, 2, 20, and 21.
 * Is used to calculate the index number of objects.
 *
 * \param bandNumber The DECT band index.
 * \return The first logical channel number for the band.
 * \throws Aborts if the band number is invalid or unsupported.
 */
uint16_t
Dect2020ChannelManager::GetFirstValidChannelNumber(uint8_t bandNumber)
{
    switch (bandNumber)
    {
    case 1:

        return 1657;
        break;

    case 2:

        return 1680;
        break;

    case 20:

        return 3416;
        break;

    case 21:

        return 3357;
        break;

    default:
        NS_FATAL_ERROR("Invalid band number. Supported bands are 1, 2, 20, and 21.");
    }
}

/**
 * \brief Determines the DECT band number corresponding to a given channel number.
 *
 * Iterates through predefined band ranges and returns the matching band.
 * 
 * \param channelNumber The logical DECT channel number.
 * \return The associated band number, or 255 if no matching band is found.
 */
uint8_t
Dect2020ChannelManager::GetBandNumber(uint16_t channelNumber)
{
    struct BandChannelRange
    {
        uint8_t bandNumber;
        uint16_t nStart;
        uint16_t nEnd;
    };

    static const std::vector<BandChannelRange> bandMap = {{1, 1657, 1677},
                                                          {2, 1680, 1700},
                                                          {20, 3416, 3645},
                                                          {21, 3357, 3414}};

    for (const auto& band : bandMap)
    {
        if (channelNumber >= band.nStart && channelNumber <= band.nEnd)
        {
            return band.bandNumber;
        }
    }

    return -1; // Invalid band number
}

std::map<uint8_t, Ptr<SpectrumModel>> Dect2020ChannelManager::m_bandModels;
std::map<uint8_t, Ptr<SpectrumValue>> Dect2020ChannelManager::m_channelOccupancy;

/**
 * \brief Retrieves or creates the SpectrumModel for a specific DECT band.
 *
 * If no SpectrumModel exists yet for the given band, it is created based on the center
 * frequencies of the band's channels as defined in \ref GetBandParameters.
 *
 * Additionally, this method initializes the corresponding SpectrumValue (PSD) representing
 * the baseline channel occupancy (e.g., thermal noise floor) and stores it for later access.
 *
 * \note The noise floor is initialized with −111 dBm per channel, corresponding to
 *       a 1.728 MHz DECT channel bandwidth (thermal noise approximation).
 *
 * \remark
 * This method is primarily used by Dect2020Phy and Dect2020SpectrumSignalParameters
 * to:
 *  - allocate transmit PSDs during StartTx,
 *  - evaluate received power levels (RSSI) during StartRx,
 *
 * \param bandId The DECT band index (e.g., 1, 2, 20, 21).
 * \return A pointer to the SpectrumModel associated with the given band.
 */
Ptr<SpectrumModel>
Dect2020ChannelManager::GetSpectrumModel(uint8_t bandId)
{
    // Check if a SpectrumModel for this band already exists
    auto iterator = m_bandModels.find(bandId);
    if (iterator != m_bandModels.end())
    {
        return iterator->second;
    }

    // Spectrum Model for this band does not exist --> create it
    BandParameters bp = GetBandParameters(bandId);

    // Generate a list of center frequencies, one per DECT channel
    std::vector<double> frequencies;
    for (int i = 0; i < bp.nEnd - bp.nStart + 1; i++)
    {
        frequencies.push_back(bp.startFrequency + i * bp.frequencyStep);
    }

    // Create the SpectrumModel from the calculated frequencies
    Ptr<SpectrumModel> model = Create<SpectrumModel>(frequencies);
    m_bandModels[bandId] = model;

    // Create an initial PSD (Power Spectral Density) entry for channel occupancy
    Ptr<SpectrumValue> psd = Create<SpectrumValue>(model);

    // Add a realistic noise floor for each channel (e.g. −111 dBm per channel)
    // This corresponds to thermal noise: −174 dBm/Hz + 10*log10(channel bandwidth in Hz)
    for (uint32_t i = 0; i < psd->GetSpectrumModel()->GetNumBands(); ++i)
    {
        double noiseDbm = -111.0;
        double noiseWatt = DbmToW(noiseDbm); // Convert dBm to linear Watt
        (*psd)[i] = noiseWatt;               // Set the baseline noise power
    }

    // Store the initialized PSD
    m_channelOccupancy[bandId] = psd;

    NS_LOG_INFO("Created SpectrumModel for Band " << static_cast<int>(bandId) << " with "
                                                  << psd->GetSpectrumModel()->GetNumBands()
                                                  << " channels and noise floor.");

    return model;
}

/**
 * Add signal power to the PSD of a given channel.
 * Creates the SpectrumValue if it doesn't exist.
 *
 * \param channelId The logical DECT channel number.
 * \param powerWatt Power to add (in Watts)
 */
void
Dect2020ChannelManager::AddSpectrumPowerToChannel(uint16_t channelId, double powerWatt)
{
    uint8_t bandNumber = GetBandNumber(channelId);
    uint16_t channelIndex = channelId - GetFirstValidChannelNumber(bandNumber);

    auto it = m_channelOccupancy.find(bandNumber);

    if (it == m_channelOccupancy.end() || it->second == nullptr)
    {
        NS_LOG_WARN("SpectrumValue for band " << static_cast<int>(bandNumber)
                                              << " not initialized. Creating now.");

        Ptr<SpectrumModel> model = GetSpectrumModel(bandNumber);
        it = m_channelOccupancy.find(bandNumber);
    }

    Ptr<SpectrumValue> psd = it->second;
    (*psd)[channelIndex] += powerWatt;
}

/**
 * Subtract signal power from the PSD of a given channel.
 * Creates the SpectrumValue if it doesn't exist.
 *
 * \param channelId The logical DECT channel number.
 * \param powerWatt Power to remove (in Watts)
 */
void
Dect2020ChannelManager::RemoveSpectrumPowerFromChannel(uint16_t channelId, double powerWatt)
{
    uint8_t bandNumber = GetBandNumber(channelId);
    uint16_t channelIndex = channelId - GetFirstValidChannelNumber(bandNumber);

    auto it = m_channelOccupancy.find(bandNumber);

    if (it == m_channelOccupancy.end() || it->second == nullptr)
    {
        NS_LOG_WARN("SpectrumValue for band " << static_cast<int>(bandNumber)
                                              << " not initialized. Creating now.");

        Ptr<SpectrumModel> model = GetSpectrumModel(bandNumber);
        it = m_channelOccupancy.find(bandNumber);
    }

    Ptr<SpectrumValue> psd = it->second;

    (*psd)[channelIndex] -= powerWatt;

    // NS_LOG_INFO("Removed " << powerWatt << " dBm to channel " << channelId << " (band "
    //                        << (int)bandNumber << ", index " << channelIndex << ")");
}

/**
 * Get current RSSI in dBm for a given channel.
 * Returns noise + interference level.
 * 
  * \param channelId The logical DECT channel number.
 */
double
Dect2020ChannelManager::GetRssiDbm(uint16_t channelId)
{
    uint8_t bandNumber = GetBandNumber(channelId);
    uint16_t channelIndex = channelId - GetFirstValidChannelNumber(bandNumber);

    auto it = m_channelOccupancy.find(bandNumber);

    if (it == m_channelOccupancy.end() || it->second == nullptr)
    {
        NS_LOG_WARN("SpectrumValue for band " << static_cast<int>(bandNumber)
                                              << " not initialized. Creating now.");

        Ptr<SpectrumModel> model = GetSpectrumModel(bandNumber);
        it = m_channelOccupancy.find(bandNumber);
    }

    Ptr<SpectrumValue> psd = it->second;
    return WToDbm((*psd)[channelIndex]);
}

/**
 * Convert dBm to Watt.
 */
double
Dect2020ChannelManager::DbmToW(double dBm)
{
    return std::pow(10.0, 0.1 * (dBm - 30.0));
}

/**
 * Convert Watt to dBm.
 */
double
Dect2020ChannelManager::WToDbm(double w)
{
    if (w <= 0.0)
    {
        // Invalid power value
        return -150;
    }
    return 10.0 * std::log10(w) + 30.0;
}

} // namespace ns3
