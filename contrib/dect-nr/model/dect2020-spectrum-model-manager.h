#ifndef DECT2020_SPECTRUM_MODEL_MANAGER_H
#define DECT2020_SPECTRUM_MODEL_MANAGER_H

#include "ns3/dect2020-operating-band.h"
#include "ns3/spectrum-model.h"

#include <map>

namespace ns3
{

class Dect2020SpectrumModelManager
{
  public:
    static Ptr<SpectrumModel> GetSpectrumModel(uint8_t bandId);
    static void AddSpectrumPowerToChannel(uint16_t channelId, double powerWatt);
    static void RemoveSpectrumPowerFromChannel(uint16_t channelId, double powerWatt);
    static double GetSpectrumValue(uint16_t channelId);
    static double DbmToW(double dBm);
    static double WToDbm(double w);

    static std::map<uint8_t, Ptr<SpectrumValue>> channelOccupancy;

  private:
    static std::map<uint8_t, Ptr<SpectrumModel>> m_bandModels;
};

} // namespace ns3

#endif