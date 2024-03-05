#include "dect-phy.h"
#include "ns3/net-device.h"

namespace ns3
{
namespace dect2020
{

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

Ptr<NetDevice>
DectPhy::GetDevice() const
{
    return m_device;
}

Ptr<Dect2020Channel>
DectPhy::GetChannel() const
{
    return m_channel;
}

} // namespace dect2020

} // namespace ns3

