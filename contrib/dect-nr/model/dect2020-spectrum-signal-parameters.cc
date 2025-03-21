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
    Ptr<Dect2020SpectrumSignalParameters> copy = Create<Dect2020SpectrumSignalParameters>();

    copy->duration = this->duration;
    copy->txPhy = this->txPhy;
    copy->txAntenna = this->txAntenna;
    copy->psd = this->psd ? this->psd->Copy() : nullptr;

    copy->txPacket = this->txPacket ? this->txPacket->Copy() : nullptr;

    return copy;
    // return nullptr;
}

} // namespace ns3