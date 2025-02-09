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

Dect2020OperatingBand::Dect2020OperatingBand()
{
    m_bandParameters = InitializeBandParameters(1);
}

Dect2020OperatingBand::Dect2020OperatingBand(uint8_t bandNumber)
{
    m_bandParameters = InitializeBandParameters(bandNumber);
}

BandParameters
Dect2020OperatingBand::InitializeBandParameters(uint8_t bandNumber)
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

// #ETSI 103 636-2 V1.5.1 Section 5.4.2
double
Dect2020OperatingBand::CalculateCenterFrequency(uint8_t bandNumber, uint32_t channelNumber)
{
    double centerFrequency = 0;

    if((bandNumber >= 1 && bandNumber <= 12) || bandNumber == 22)
    {
        double f0 = 450.144e6;  // F0 in Hz
        double channelRaster = 0.864e6;

        centerFrequency = f0 + (channelNumber * channelRaster);
    }
    // TODO: Add more Bands


    return centerFrequency;
}

std::vector<ChannelInfo>
Dect2020OperatingBand::GetAvailableChannels() const
{
    // TODO
    return std::vector<ChannelInfo>();
}

} // namespace ns3