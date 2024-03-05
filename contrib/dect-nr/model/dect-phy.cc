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

Ptr<SimpleNetDevice>
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

