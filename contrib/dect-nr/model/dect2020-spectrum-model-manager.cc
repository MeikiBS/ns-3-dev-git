#include "ns3/dect2020-spectrum-model-manager.h"

NS_LOG_COMPONENT_DEFINE("Dect2020SpectrumModelManager");

namespace ns3
{

std::map<uint8_t, Ptr<SpectrumModel>> Dect2020SpectrumModelManager::m_bandModels;
std::map<uint8_t, Ptr<SpectrumValue>> Dect2020SpectrumModelManager::channelOccupancy;

Ptr<SpectrumModel>
Dect2020SpectrumModelManager::GetSpectrumModel(uint8_t bandId)
{
    auto iterator = m_bandModels.find(bandId);
    if (iterator != m_bandModels.end())
    {
        return iterator->second;
    }

    // Spectrum Model for this band does not exist --> create
    Dect2020OperatingBand band(bandId);
    BandParameters bp = band.m_bandParameters;

    std::vector<double> frequencies;
    for (int i = 0; i < bp.nEnd - bp.nStart + 1; i++)
    {
        frequencies.push_back(bp.startFrequency + i * bp.frequencyStep);
    }

    Ptr<SpectrumModel> model = Create<SpectrumModel>(frequencies);
    m_bandModels[bandId] = model;

    Ptr<SpectrumValue> psd = Create<SpectrumValue>(model);
    channelOccupancy[bandId] = psd;

    NS_LOG_INFO("Created SpectrumModel for Band " << static_cast<int>(bandId));

    return model;
}

/**
 * \param powerWatt Power to add (in **Watts**)
 */
void
Dect2020SpectrumModelManager::AddSpectrumPowerToChannel(uint16_t channelId, double powerWatt)
{
    uint8_t bandNumber = Dect2020OperatingBand::GetBandNumber(channelId);
    uint16_t channelIndex =
        channelId - Dect2020OperatingBand::GetFirstValidChannelNumber(bandNumber);

    auto it = channelOccupancy.find(bandNumber);

    if (it == channelOccupancy.end() || it->second == nullptr)
    {
        NS_LOG_WARN("SpectrumValue for band " << static_cast<int>(bandNumber)
                                              << " not initialized. Creating now.");

        Ptr<SpectrumModel> model = GetSpectrumModel(bandNumber);
        it = channelOccupancy.find(bandNumber);
    }

    Ptr<SpectrumValue> psd = it->second;
    (*psd)[channelIndex] += powerWatt;

    NS_LOG_INFO("Added " << powerWatt << " dBm to channel " << channelId << " (band "
                         << (int)bandNumber << ", index " << channelIndex << ")");
}

/**
 * \param powerWatt Power to remove (in **Watts**)
 */
void
Dect2020SpectrumModelManager::RemoveSpectrumPowerFromChannel(uint16_t channelId, double powerWatt)
{
    uint8_t bandNumber = Dect2020OperatingBand::GetBandNumber(channelId);
    uint16_t channelIndex =
        channelId - Dect2020OperatingBand::GetFirstValidChannelNumber(bandNumber);

    auto it = channelOccupancy.find(bandNumber);

    if (it == channelOccupancy.end() || it->second == nullptr)
    {
        NS_LOG_WARN("SpectrumValue for band " << static_cast<int>(bandNumber)
                                              << " not initialized. Creating now.");

        Ptr<SpectrumModel> model = GetSpectrumModel(bandNumber);
        it = channelOccupancy.find(bandNumber);
    }

    Ptr<SpectrumValue> psd = it->second;
    // ############ DEBUG
    auto currentPsd = (*psd)[channelIndex];
    NS_LOG_INFO(currentPsd);
    // ############
    (*psd)[channelIndex] -= powerWatt;

    NS_LOG_INFO("Removed " << powerWatt << " dBm to channel " << channelId << " (band "
                           << (int)bandNumber << ", index " << channelIndex << ")");
}

double
Dect2020SpectrumModelManager::GetRssiDbm(uint16_t channelId)
{
    uint8_t bandNumber = Dect2020OperatingBand::GetBandNumber(channelId);
    uint16_t channelIndex =
        channelId - Dect2020OperatingBand::GetFirstValidChannelNumber(bandNumber);

    auto it = channelOccupancy.find(bandNumber);

    if (it == channelOccupancy.end() || it->second == nullptr)
    {
        NS_LOG_WARN("SpectrumValue for band " << static_cast<int>(bandNumber)
                                              << " not initialized. Creating now.");

        Ptr<SpectrumModel> model = GetSpectrumModel(bandNumber);
        it = channelOccupancy.find(bandNumber);
    }

    Ptr<SpectrumValue> psd = it->second;
    return WToDbm((*psd)[channelIndex]);
}

double
Dect2020SpectrumModelManager::DbmToW(double dBm)
{
    return std::pow(10.0, 0.1 * (dBm - 30.0));
}

double
Dect2020SpectrumModelManager::WToDbm(double w)
{
    if(w <= 0.0)
    {
        // Invalid power value
        return -150;
    }
    return 10.0 * std::log10(w) + 30.0;
}

} // namespace ns3