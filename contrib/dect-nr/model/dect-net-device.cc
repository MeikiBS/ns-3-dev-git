#include "dect-net-device.h"
#include "ns3/log.h"
#include "ns3/node.h"

namespace ns3
{
namespace dect2020
{

NS_LOG_COMPONENT_DEFINE("DectNetDevice");

NS_OBJECT_ENSURE_REGISTERED(DectNetDevice);

DectNetDevice::DectNetDevice()
{
    NS_LOG_FUNCTION_NOARGS();
}

DectNetDevice::~DectNetDevice()
{
    NS_LOG_FUNCTION_NOARGS();
}

TypeId
DectNetDevice::GetTypeId()
{
    static TypeId tid = TypeId("ns3::dect2020::DectNetDevice")
                            .SetParent<NetDevice>();
    return tid;
}

void
DectNetDevice::SetMac(Ptr<DectMac> mac)
{
}

void
DectNetDevice::SetPhy(Ptr<DectPhy> phy)
{
}

/******************************************
 *    Methods inherited from NetDevice    *
 ******************************************/

Ptr<Channel>
DectNetDevice::GetChannel() const
{
    NS_LOG_FUNCTION(this);
    return m_phy->GetChannel();
}

Ptr<Dect2020Channel>
DectNetDevice::DoGetChannel() const
{
    NS_LOG_FUNCTION(this);
    return m_phy->GetChannel();
}

void
DectNetDevice::SetIfIndex(const uint32_t index)
{
    NS_LOG_FUNCTION(this << index);
}

uint32_t
DectNetDevice::GetIfIndex() const
{
    NS_LOG_FUNCTION(this);

    return 0;
}

void
DectNetDevice::SetAddress(Address address)
{
    NS_LOG_FUNCTION(this);
}

Address
DectNetDevice::GetAddress() const
{
    NS_LOG_FUNCTION(this);

    return Address();
}

bool
DectNetDevice::SetMtu(const uint16_t mtu)
{
    NS_ABORT_MSG("Unsupported");

    return false;
}

uint16_t
DectNetDevice::GetMtu() const
{
    NS_LOG_FUNCTION(this);

    return 0;
}

bool
DectNetDevice::IsLinkUp() const
{
    NS_LOG_FUNCTION(this);

    return m_phy != nullptr;
}

void
DectNetDevice::AddLinkChangeCallback(Callback<void> callback)
{
    NS_LOG_FUNCTION(this);
}

bool
DectNetDevice::IsBroadcast() const
{
    NS_LOG_FUNCTION(this);

    return true;
}

Address
DectNetDevice::GetBroadcast() const
{
    NS_LOG_FUNCTION(this);

    return Address();
}

bool
DectNetDevice::IsMulticast() const
{
    NS_LOG_FUNCTION(this);

    return true;
}

Address
DectNetDevice::GetMulticast(Ipv4Address multicastGroup) const
{
    NS_ABORT_MSG("Unsupported");

    return Address();
}

Address
DectNetDevice::GetMulticast(Ipv6Address addr) const
{
    NS_LOG_FUNCTION(this);

    return Address();
}

bool
DectNetDevice::IsBridge() const
{
    NS_LOG_FUNCTION(this);

    return false;
}

bool
DectNetDevice::IsPointToPoint() const
{
    NS_LOG_FUNCTION(this);

    return false;
}

bool
DectNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << dest << protocolNumber);

    // Fallback to the vanilla Send method
   // Send(packet);

    return true;
}

bool
DectNetDevice::SendFrom(Ptr<Packet> packet,
                        const Address& source,
                        const Address& dest,
                        uint16_t protocolNumber)

{
    NS_ABORT_MSG("Unsupported");

    return false;
}

Ptr<Node>
DectNetDevice::GetNode() const
{
    NS_LOG_FUNCTION(this);

    return m_node;
}

void
DectNetDevice::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this);

    m_node = node;
    //CompleteConfig();
}

bool
DectNetDevice::NeedsArp() const
{
    NS_LOG_FUNCTION(this);

    return true;
}

void
DectNetDevice::SetReceiveCallback(ReceiveCallback cb)
{
    NS_LOG_FUNCTION_NOARGS();
    //m_receiveCallback = cb;
}

void
DectNetDevice::SetPromiscReceiveCallback(PromiscReceiveCallback cb)
{
    NS_LOG_FUNCTION_NOARGS();
}

bool
DectNetDevice::SupportsSendFrom() const
{
    NS_LOG_FUNCTION_NOARGS();

    return false;
}


} // namespace dect2020
} // namespace ns3
