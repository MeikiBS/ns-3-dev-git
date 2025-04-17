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

void
Dect2020SpectrumModelManager::SetSpectrumValue(uint16_t channelId, double value)
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
    (*psd)[channelIndex] = value;
}
} // namespace ns3