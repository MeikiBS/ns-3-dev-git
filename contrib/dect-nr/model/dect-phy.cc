#include "dect-phy.h"


namespace ns3
{
namespace dect2020
{

NS_OBJECT_ENSURE_REGISTERED(DectPhy);


TypeId
DectPhy::GetTypeId()
{
    static TypeId tid = TypeId("ns3::dect2020::DectPhy")
                            .SetParent<Object>();
    return tid;
}

DectPhy::DectPhy()
{
}

DectPhy::~DectPhy()
{
}

void
DectPhy::SetDevice(Ptr<NetDevice> device)
{
    m_device = device;
}

Ptr<NetDevice>
DectPhy::GetDevice() const
{
    return m_device;
}

void
DectPhy::SetChannel(Ptr<SpectrumChannel> channel)
{
    m_channel = channel;
}

Ptr<SpectrumChannel>
DectPhy::GetChannel() const
{
    return m_channel;
}

} // namespace dect2020

} // namespace ns3

