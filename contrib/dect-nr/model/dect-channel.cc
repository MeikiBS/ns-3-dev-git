#include "dect-channel.h"



namespace ns3
{
namespace dect2020
{
TypeId
Dect2020Channel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::Dect2020Channel").SetParent<Channel>().SetGroupName("dect2020");
    return tid;
}

Dect2020Channel::Dect2020Channel()
{
}

Dect2020Channel::~Dect2020Channel()
{
    m_phyList.clear();
}

std::size_t
Dect2020Channel::GetNDevices() const
{
    return m_phyList.size();
}

Ptr<NetDevice>
Dect2020Channel::GetDevice(std::size_t i) const
{
    return m_phyList[i]->GetDevice()->GetObject<NetDevice>();
}

void
Dect2020Channel::Add(Ptr<DectPhy> phy)
{

}

void
Dect2020Channel::Add(Ptr<DectMac> mac)
{

}

void
Send(Ptr<DectPhy> sender, Ptr<Packet> packet, Time duration, double frequencyMHz)
{

}

} // namespace dect2020
} // namespace ns3

