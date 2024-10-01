/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-channel.h"
#include "dect2020-net-device.h"
#include "dect2020-phy.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("Dect2020Channel");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Channel);

TypeId
Dect2020Channel::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020Channel")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020Channel>()
                            .AddAttribute("Delay",
                                          "Signalverzögerung im Kanal",
                                          TimeValue(NanoSeconds(0)),
                                          MakeTimeAccessor(&Dect2020Channel::m_delay),
                                          MakeTimeChecker());
    return tid;
}

Dect2020Channel::Dect2020Channel()
    : m_delay(NanoSeconds(0))
{
    NS_LOG_FUNCTION(this);
}

Dect2020Channel::~Dect2020Channel()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020Channel::AddPhy(Ptr<Dect2020Phy> phy)
{
    NS_LOG_FUNCTION(this << phy);
    m_phyList.push_back(phy);
}

void
Dect2020Channel::Transmit(Ptr<Packet> packet, Ptr<Dect2020Phy> sender)
{
    NS_LOG_FUNCTION(this << packet << sender);

    // Pakete an alle angeschlossenen PHYs senden, außer dem Sender
    for (auto phy : m_phyList)
    {
        if (phy != sender)
        {
            // Planen des Empfangs mit der Kanalverzögerung
            Simulator::Schedule(m_delay, &Dect2020Phy::Receive, phy, packet->Copy());
        }
    }
}

std::size_t
Dect2020Channel::GetNDevices(void) const
{
    return m_phyList.size();
}

Ptr<NetDevice>
Dect2020Channel::GetDevice(std::size_t i) const
{
    if (i < m_phyList.size())
    {
        return m_phyList[i]->GetNetDevice();
    }
    else
    {
        return nullptr;
    }
}

} // namespace ns3
