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
}

Dect2020Channel::~Dect2020Channel()
{
    NS_LOG_FUNCTION(this);
}
} // namespace ns3
