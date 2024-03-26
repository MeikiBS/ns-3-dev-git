#include "dect-net-device.h"

namespace ns3
{
namespace dect2020
{
NS_LOG_COMPONENT_DEFINE("DectNetDevice");

NS_OBJECT_ENSURE_REGISTERED(DectNetDevice);

DectNetDevice::DectNetDevice()
{
    NS_LOG_FUNCTION(this);

    m_mac = CreateObject<EndDeviceDect2020Mac>();
    m_phy = CreateObject<DectPhy>();
}

DectNetDevice::~DectNetDevice()
{
}

TypeId
DectNetDevice::GetTypeId()
{
    static TypeId tid = TypeId("ns3::dect2020::DectNetDevice").SetParent<NetDevice>();
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
DectNetDevice::SetChannel(Ptr<SpectrumChannel> channel)
{
    m_channel = channel;
}

void
DectNetDevice::Test()
{
    std::cout << "Test" << std::endl;
}

// inherited from NetDevice

void
DectNetDevice::SetIfIndex(const uint32_t index)
{
    // TODO
}

uint32_t
DectNetDevice::GetIfIndex() const
{
    // TODO
    return 0;
}

Ptr<Channel>
DectNetDevice::GetChannel() const
{
    return m_channel;
}

void
DectNetDevice::SetAddress(Address address)
{
    // TODO
}

Address
DectNetDevice::GetAddress() const
{
    // TODO
    return Address();
}

bool
DectNetDevice::SetMtu(const uint16_t mtu)
{
    // TODO
    return true;
}

uint16_t
DectNetDevice::GetMtu() const
{
    // TODO
    return 0;
}

bool
DectNetDevice::IsLinkUp() const
{
    // TODO
    return true; // Dummy-Rückgabewert
}

void
DectNetDevice::AddLinkChangeCallback(Callback<void> callback)
{
    // TODO
}

bool
DectNetDevice::IsBroadcast() const
{
    // TODO
    return false; // Dummy-Rückgabewert
}

Address
DectNetDevice::GetBroadcast() const
{
    // TODO
    return Address();
}

bool
DectNetDevice::IsMulticast() const
{
    // TODO
    return true;
}

Address
DectNetDevice::GetMulticast(Ipv4Address addr) const
{
    // TODO: Implementiere die Methode für IPv6
    return Address(); // Dummy-Rückgabewert
}

Address
DectNetDevice::GetMulticast(Ipv6Address addr) const
{
    // TODO: Implementiere die Methode für IPv6
    return Address(); // Dummy-Rückgabewert
}

bool
DectNetDevice::IsBridge() const
{
    // TODO: Implementiere die Methode
    return false; // Dummy-Rückgabewert
}

bool
DectNetDevice::IsPointToPoint() const
{
    // TODO: Implementiere die Methode
    return false; // Dummy-Rückgabewert
}

bool
DectNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    // Die vorhandene Implementierung kann beibehalten oder angepasst werden
    return false; // Dummy-Rückgabewert
}

bool
DectNetDevice::SendFrom(Ptr<Packet> packet,
                        const Address& source,
                        const Address& dest,
                        uint16_t protocolNumber)
{
    // TODO: Implementiere die Methode
    return false; // Dummy-Rückgabewert
}

Ptr<Node>
DectNetDevice::GetNode() const
{
    // TODO: Implementiere die Methode
    return nullptr; // Dummy-Rückgabewert
}

void
DectNetDevice::SetNode(Ptr<Node> node)
{
    // TODO: Implementiere die Methode
}

bool
DectNetDevice::NeedsArp() const
{
    // TODO: Implementiere die Methode
    return false; // Dummy-Rückgabewert
}

void
DectNetDevice::SetReceiveCallback(ReceiveCallback cb)
{
    // TODO: Implementiere die Methode
}

void
DectNetDevice::SetPromiscReceiveCallback(PromiscReceiveCallback cb)
{
    // TODO: Implementiere die Methode
}

bool
DectNetDevice::SupportsSendFrom() const
{
    // TODO: Implementiere die Methode
    return false;  // Dummy-Rückgabewert
}

} // namespace dect2020
} // namespace ns3
