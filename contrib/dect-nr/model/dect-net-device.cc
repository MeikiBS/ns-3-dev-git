#include "dect-net-device.h"


namespace ns3
{
namespace dect2020
{

NS_OBJECT_ENSURE_REGISTERED(DectNetDevice);

DectNetDevice::DectNetDevice()
{
}

DectNetDevice::~DectNetDevice()
{
    
}

TypeId
DectNetDevice::GetTypeId()
{
    static TypeId tid = TypeId("ns3::dect2020::DectNetDevice")
                            .SetParent<SimpleNetDevice>();
    return tid;
}

void
DectNetDevice::SetMac(Ptr<DectMac> mac)
{
    m_mac = mac;
}

Ptr<DectMac>
DectNetDevice::GetMac() const
{
    return m_mac;
}

void
DectNetDevice::SetPhy(Ptr<DectPhy> phy)
{
    m_phy = phy;
}

void
DectNetDevice::SetChannel(Ptr<Dect2020Channel> channel)
{
    m_channel = channel;
}

bool
DectNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    m_channel = nullptr;
    return true;
}

void
DectNetDevice::Test()
{
    std::cout << "Test" << std::endl;
}



} // namespace dect2020
} // namespace ns3
