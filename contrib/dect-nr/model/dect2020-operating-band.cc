#include "ns3/dect2020-operating-band.h"

#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("Dect2020OperatingBand");

namespace ns3
{
NS_OBJECT_ENSURE_REGISTERED(Dect2020OperatingBand);

TypeId
Dect2020OperatingBand::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020OperatingBand")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020OperatingBand>();

    return tid;
}

Dect2020OperatingBand::Dect2020OperatingBand(uint8_t bandNumber, OperatingChannelBandwith operatingChannelBandwidth)
{
    m_bandParameters = InitializeBandParameters(bandNumber, operatingChannelBandwidth);
}

BandParameters
Dect2020OperatingBand::InitializeBandParameters(uint8_t bandNumber, OperatingChannelBandwith operatingChannelBandwith)
{
    BandParameters bandParameters;
    bandParameters.bandNumber = bandNumber;

    switch(bandNumber)
    {
        case 1:
            bandParameters.startFrequency = 1880e6;
            bandParameters.stopFrequency = 1900e6;
            
            if(operatingChannelBandwith == OperatingChannelBandwith::BANDWITH_I)
            {
                bandParameters.frequencyStep = 1.728e6;
            }
            else
            {
                bandParameters.frequencyStep = 0.864e6;
            }

            bandParameters.nStart = 1655;
            bandParameters.nEnd = 1679;

            break;
            
    }
}

} // namespace ns3