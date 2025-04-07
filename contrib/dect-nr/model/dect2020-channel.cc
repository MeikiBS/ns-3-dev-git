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
                            .AddConstructor<Dect2020Channel>();
    return tid;
}

Dect2020Channel::Dect2020Channel()
{
    NS_LOG_FUNCTION(this);

    std::vector<Slot> m_slots;
    std::vector<Slot> m_slotsLastFrame;
}

Dect2020Channel::~Dect2020Channel()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020Channel::AddSlot(Slot slot)
{
    m_slots.push_back(slot);

    if(m_slots.size() == 24)
    {
        m_slotsLastFrame = m_slots; // Deep copy of the fresh initialized Slot List
    }
}
} // namespace ns3
