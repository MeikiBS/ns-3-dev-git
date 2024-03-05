#include "dect-channel.h"

namespace ns3
{
namespace dect2020
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Channel);

TypeId
Dect2020Channel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::dect2020::Dect2020Channel").SetParent<SimpleChannel>().SetGroupName("dect2020");
    return tid;
}

Dect2020Channel::Dect2020Channel()
{
}

Dect2020Channel::~Dect2020Channel()
{
    m_phyList.clear();
}

void
Dect2020Channel::Send(Ptr<Packet> p,
                      uint16_t protocol,
                      Mac48Address to,
                      Mac48Address from,
                      Ptr<SimpleNetDevice> sender)
{
}

void
Dect2020Channel::Add(Ptr<SimpleNetDevice> device)
{
}

void
Dect2020Channel::BlackList(Ptr<SimpleNetDevice> from, Ptr<SimpleNetDevice> to)
{
}

void
Dect2020Channel::UnBlackList(Ptr<SimpleNetDevice> from, Ptr<SimpleNetDevice> to)
{
}


} // namespace dect2020
} // namespace ns3
