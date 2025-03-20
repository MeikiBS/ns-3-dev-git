#include "dect2020-spectrum-signal-parameters.h"

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/spectrum-signal-parameters.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020SpectrumSignalParameters");

Dect2020SpectrumSignalParameters::Dect2020SpectrumSignalParameters()
    : SpectrumSignalParameters()
{
}

Dect2020SpectrumSignalParameters::~Dect2020SpectrumSignalParameters()
{
}

Ptr<SpectrumSignalParameters>
Dect2020SpectrumSignalParameters::Copy() const
{
    // TODO
    return nullptr;
}

} // namespace ns3