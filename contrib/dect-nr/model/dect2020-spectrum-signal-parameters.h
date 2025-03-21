#ifndef DECT2020_SPECTRUM_SIGNAL_PARAMETERS
#define DECT2020_SPECTRUM_SIGNAL_PARAMETERS

#include "ns3/spectrum-signal-parameters.h"
#include "ns3/packet.h"
#include "ns3/spectrum-value.h" // Include the header for SpectrumValue
#include "ns3/spectrum-phy.h"
#include "ns3/antenna-model.h"


namespace ns3
{

class Dect2020SpectrumSignalParameters : public SpectrumSignalParameters
{
  public:
    Dect2020SpectrumSignalParameters();
    virtual ~Dect2020SpectrumSignalParameters();
    Ptr<SpectrumSignalParameters> Copy() const;

    Ptr<Packet> txPacket;

  private:
};

} // namespace ns3

#endif // DECT2020_SPECTRUM_SIGNAL_PARAMETERS
