/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-net-device.h"

#include "dect2020-channel.h"
#include "dect2020-mac.h"
#include "dect2020-phy.h"

#include "ns3/channel.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Dect2020NetDevice");
NS_OBJECT_ENSURE_REGISTERED(Dect2020NetDevice);

TypeId
Dect2020NetDevice::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020NetDevice")
                            .SetParent<NetDevice>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020NetDevice>();
    return tid;
}

Dect2020NetDevice::Dect2020NetDevice()
    : m_node(nullptr),
      m_ifIndex(0),
      m_mtu(1500),
      m_linkUp(false)
//   m_terminationPointType(PT)
{
    NS_LOG_FUNCTION(this);
}

Dect2020NetDevice::~Dect2020NetDevice()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020NetDevice::SetTerminationPointType(TerminationPointType tpt)
{
    NS_LOG_FUNCTION(this << tpt);
    if (tpt != PT && tpt != FT)
    {
        NS_LOG_ERROR(Simulator::Now().GetMilliSeconds() << ": Invalid TerminationPointType");
        return;
    }

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": " << __PRETTY_FUNCTION__ << " | "
                << "Set TerminationPointType to: " << (tpt == PT ? "PT" : "FT"));

    SetBootstrappingVariables((tpt == FT) ? true : false);


    m_terminationPointType = tpt;
}

Dect2020NetDevice::TerminationPointType
Dect2020NetDevice::GetTerminationPointType()
{
    return m_terminationPointType;
}

void
Dect2020NetDevice::DoInitialize(void)
{
    NS_LOG_FUNCTION(this);
    if (m_mac)
    {
        m_mac->SetNetDevice(this);
    }
    if (m_phy)
    {
        m_phy->SetDevice(this);
    }
    NetDevice::DoInitialize();
}

void
Dect2020NetDevice::SetIfIndex(const uint32_t index)
{
    m_ifIndex = index;
}

uint32_t
Dect2020NetDevice::GetIfIndex(void) const
{
    return m_ifIndex;
}

Ptr<Channel>
Dect2020NetDevice::GetChannel(void) const
{
    return m_phy->GetChannel();
}

void
Dect2020NetDevice::SetAddress(Address address)
{
    m_address = Mac48Address::ConvertFrom(address);
}

Address
Dect2020NetDevice::GetAddress(void) const
{
    return m_address;
}

bool
Dect2020NetDevice::SetMtu(const uint16_t mtu)
{
    m_mtu = mtu;
    return true;
}

uint16_t
Dect2020NetDevice::GetMtu(void) const
{
    return m_mtu;
}

bool
Dect2020NetDevice::IsLinkUp(void) const
{
    return m_linkUp;
}

void
Dect2020NetDevice::AddLinkChangeCallback(Callback<void> callback)
{
    m_linkChanges.ConnectWithoutContext(callback);
}

bool
Dect2020NetDevice::IsBroadcast(void) const
{
    return true;
}

Address
Dect2020NetDevice::GetBroadcast(void) const
{
    return Mac48Address("ff:ff:ff:ff:ff:ff");
}

bool
Dect2020NetDevice::IsMulticast(void) const
{
    return true;
}

Address
Dect2020NetDevice::GetMulticast(Ipv4Address multicastGroup) const
{
    return Mac48Address::GetMulticast(multicastGroup);
}

Address
Dect2020NetDevice::GetMulticast(Ipv6Address multicastGroup) const
{
    return Mac48Address::GetMulticast(multicastGroup);
}

bool
Dect2020NetDevice::IsBridge(void) const
{
    return false;
}

bool
Dect2020NetDevice::IsPointToPoint(void) const
{
    return false;
}

bool
Dect2020NetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << dest << protocolNumber);

    // TEST
    NS_LOG_INFO("Dect2020NetDevice::Send aufgerufen");

    if (!m_linkUp)
    {
        NS_LOG_ERROR("Link is down");
        return false;
    }

    // Übergabe an die MAC-Schicht
    Dect2020Mac::Dect2020PacketType packetType;
    switch (protocolNumber)
    {
    case 0:
        packetType = Dect2020Mac::Dect2020PacketType::DATA;
        break;
    case 1:
        packetType = Dect2020Mac::Dect2020PacketType::BEACON;
        break;
    case 2:
        packetType = Dect2020Mac::Dect2020PacketType::UNICAST;
        break;
    case 3:
        packetType = Dect2020Mac::Dect2020PacketType::RD_BROADCAST;
        break;
    }

    m_mac->Send(packet, dest, packetType);
    return true;
}

bool
Dect2020NetDevice::SendFrom(Ptr<Packet> packet,
                            const Address& source,
                            const Address& dest,
                            uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << source << dest << protocolNumber);

    if (!m_linkUp)
    {
        NS_LOG_ERROR("Link is down");
        return false;
    }

    // Für die minimale Implementierung ignorieren wir die Quelladresse
    return Send(packet, dest, protocolNumber);
}

Ptr<Node>
Dect2020NetDevice::GetNode(void) const
{
    return m_node;
}

void
Dect2020NetDevice::SetNode(Ptr<Node> node)
{
    m_node = node;
}

bool
Dect2020NetDevice::NeedsArp(void) const
{
    return false;
}

void
Dect2020NetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
{
    m_rxCallback = cb;
}

void
Dect2020NetDevice::SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb)
{
    m_promiscRxCallback = cb;
}

bool
Dect2020NetDevice::SupportsSendFrom(void) const
{
    return true;
}

void
Dect2020NetDevice::SetMac(Ptr<Dect2020Mac> mac)
{
    m_mac = mac;
    m_mac->SetNetDevice(this);
}

Ptr<Dect2020Mac>
Dect2020NetDevice::GetMac(void) const
{
    return m_mac;
}

void
Dect2020NetDevice::SetPhy(Ptr<Dect2020Phy> phy)
{
    m_phy = phy;
    m_phy->SetDevice(this);
}

Ptr<Dect2020Phy>
Dect2020NetDevice::GetPhy(void) const
{
    return m_phy;
}

void
Dect2020NetDevice::Receive(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020NetDevice::Receive() aufgerufen von Device 0x" << std::hex
    //             << this->m_mac->GetLongRadioDeviceId());

    if (!m_rxCallback.IsNull())
    {
        // Für die minimale Implementierung setzen wir die Protokollnummer auf 0
        m_rxCallback(this, packet, 0, GetAddress());
    }
}

bool
ReceivePacket(Ptr<NetDevice> device,
              Ptr<const Packet> packet,
              uint16_t protocol,
              const Address& sender)
{
    NS_LOG_UNCOND(Simulator::Now().GetMilliSeconds()
                  << ": Gerät 2 hat ein Paket empfangen von " << Mac48Address::ConvertFrom(sender));
    return true; // Paket wurde erfolgreich empfangen
}

void
Dect2020NetDevice::SetLinkUp(void)
{
    NS_LOG_FUNCTION(this);
    if (!m_linkUp)
    {
        m_linkUp = true;
        m_linkChanges();
    }
}

void
Dect2020NetDevice::SetLinkDown(void)
{
    NS_LOG_FUNCTION(this);
    if (m_linkUp)
    {
        m_linkUp = false;
        m_linkChanges();
    }
}

void
Dect2020NetDevice::SetBandNumber(uint8_t bandNumber)
{
    if (bandNumber > 0 && bandNumber <= 22)
    {
        m_bandNumber = bandNumber;
    }
    else
    {
        NS_LOG_ERROR("Band number must be between 1 and 22");
    }
}

uint8_t
Dect2020NetDevice::GetBandNumber(void) const
{
    return m_bandNumber;
}

void
Dect2020NetDevice::SetBootstrappingVariables(bool isFT)
{
    m_numOfPHYCapabilities = 0;
    m_release = 2;
    m_supportGroupAssignment = false;
    m_supportPaging = false;
    m_operatingModes = (isFT ? 1 : 0);
    m_mesh = false;
    m_scheduledAccessDataTransfer = false;              
    m_macSecurity = false;             
    m_dlcServiceType = 0;          
    m_rdPowerClass = 0;            
    m_maxNssFoRx = 0;         
    m_rxForTxDiversity = 0;   
    m_rxGain = 5;   // 0 dB             
    m_maxMcs = 11;             
    m_softBufferSize = 0;     
    m_numOfHarqProcesses = 2; 
    m_harqFeedbackDelay = 2;
    m_dDelay = true;
    m_halfDulp = false;
}

} // namespace ns3
