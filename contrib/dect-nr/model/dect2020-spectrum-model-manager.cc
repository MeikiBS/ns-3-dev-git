#include "ns3/dect2020-spectrum-model-manager.h"

NS_LOG_COMPONENT_DEFINE("Dect2020SpectrumModelManager");

namespace ns3
{

std::map<uint8_t, Ptr<const SpectrumModel>> Dect2020SpectrumModelManager::m_bandModels;

Ptr<const SpectrumModel>
Dect2020SpectrumModelManager::GetSpectrumModel(uint8_t bandId)
{
    auto iterator = m_bandModels.find(bandId);
    if (iterator != m_bandModels.end())
    {
        return iterator->second;
    }

    // Model for this band does not exist --> create
    Dect2020OperatingBand band(bandId);
    BandParameters bp = band.m_bandParameters;

    std::vector<double> frequencies;
    for (int i = 0; i < bp.nEnd - bp.nStart + 1; i++)
    {
        frequencies.push_back(bp.startFrequency + i * bp.frequencyStep);
    }

    Ptr<const SpectrumModel> model = Create<const SpectrumModel>(frequencies);
    m_bandModels[bandId] = model;

    NS_LOG_INFO("Created SpectrumModel for Band " << static_cast<int>(bandId));

    return model;
}

} // namespace ns3