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

  private:
    static std::map<uint8_t, Ptr<SpectrumModel>> m_bandModels;
};

} // namespace ns3

#endif