/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-phy.h"

#include "dect2020-channel.h"
#include "dect2020-mac.h"
#include "dect2020-net-device.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("Dect2020Phy");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Phy);

TypeId
Dect2020Phy::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020Phy")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020Phy>()
                            .AddAttribute("TxDelay",
                                          "Verzögerung beim Senden von Paketen",
                                          TimeValue(NanoSeconds(0)),
                                          MakeTimeAccessor(&Dect2020Phy::m_txDelay),
                                          MakeTimeChecker())
                            .AddAttribute("RxDelay",
                                          "Verzögerung beim Empfangen von Paketen",
                                          TimeValue(NanoSeconds(0)),
                                          MakeTimeAccessor(&Dect2020Phy::m_rxDelay),
                                          MakeTimeChecker())
        // Hier können weitere Attribute und Trace-Quellen hinzugefügt werden
        ;
    return tid;
}

Dect2020Phy::Dect2020Phy()
    : m_txDelay(NanoSeconds(0)),
      m_rxDelay(NanoSeconds(0))
{
    NS_LOG_FUNCTION(this);
}

Dect2020Phy::~Dect2020Phy()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020Phy::SetMac(Ptr<Dect2020Mac> mac)
{
    NS_LOG_FUNCTION(this << mac);
    m_mac = mac;
}

void
Dect2020Phy::SetNetDevice(Ptr<Dect2020NetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
}

void
Dect2020Phy::SetChannel(Ptr<Dect2020Channel> channel)
{
    NS_LOG_FUNCTION(this << channel);
    m_channel = channel;
}

Ptr<Dect2020Channel>
Dect2020Phy::GetChannel(void) const
{
    return m_channel;
}

Ptr<Dect2020NetDevice>
Dect2020Phy::GetNetDevice(void) const
{
    return m_device;
}

void
Dect2020Phy::Send(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // Trace-Aufruf
    m_phyTxBeginTrace(packet);

    // Sendeverzögerung
    Simulator::Schedule(m_txDelay, &Dect2020Channel::Transmit, m_channel, packet, this);
}

void
Dect2020Phy::Receive(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // Empfangsverzögerung
    Simulator::Schedule(m_rxDelay, &Dect2020Phy::ReceiveDelayed, this, packet);
}

void
Dect2020Phy::ReceiveDelayed(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // Trace-Aufruf
    m_phyRxEndTrace(packet);

    // Weiterleitung an die MAC-Schicht
    m_mac->ReceiveFromPhy(packet);
}

} // namespace ns3
