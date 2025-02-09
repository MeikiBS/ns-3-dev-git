/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-phy.h"

#include "dect2020-channel.h"
#include "dect2020-mac.h"
#include "dect2020-net-device.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

#include <algorithm>

NS_LOG_COMPONENT_DEFINE("Dect2020Phy");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Phy);

std::vector<Dect2020Channel> Dect2020Phy::m_channels;

TypeId
Dect2020Phy::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020Phy")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020Phy>();
    return tid;
}

Dect2020Phy::Dect2020Phy()
    : m_currentSlot(0),
      m_currenSubslot(0)
{
    if (m_channels.empty())
    {
        InitializeChannels(1, 1);
    }
    StartFrameTimer();
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
Dect2020Phy::SetChannel(Ptr<SpectrumChannel> channel)
{
    NS_LOG_FUNCTION(this << channel);
    m_channel = channel;
}

Ptr<SpectrumChannel>
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
    // Simulator::Schedule(m_txDelay, &Dect2020Channel::Transmit, m_channel, packet, this);
}

void
Dect2020Phy::Receive(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // Empfangsverzögerung
    // Simulator::Schedule(m_rxDelay, &Dect2020Phy::ReceiveDelayed, this, packet);
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

void
Dect2020Phy::InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor)
{
    Dect2020OperatingBand operatingBand;
    BandParameters bandParams = operatingBand.InitializeBandParameters(bandNumber);

    uint32_t numChannels = (bandParams.nEnd - bandParams.nStart) + 1; // Number of Channels

    if (!(subcarrierScalingFactor == 1 || subcarrierScalingFactor == 2 ||
          subcarrierScalingFactor == 4 || subcarrierScalingFactor == 8))
    {
        NS_LOG_INFO("Subcarrier scaling factor invalid, set to 1");
        subcarrierScalingFactor = 1;
    }
    uint32_t numSubslotsPerSlot = (subcarrierScalingFactor == 1)   ? 2
                                  : (subcarrierScalingFactor == 2) ? 4
                                  : (subcarrierScalingFactor == 4) ? 8
                                                                   : 16;

    const uint32_t slotsPerFrame = 24; // #ETSI 103 636-3 V1.5.1, Section 4.4
    // uint32_t numSubslotsPerChannel = slotsPerFrame * numSubslotsPerSlot;

    m_channels.clear();
    for (uint32_t ch = 0; ch < numChannels; ch++)
    {
        int channelFrequencyNumbering = bandParams.nStart + ch;

        Dect2020Channel dect2020Channel;
        dect2020Channel.m_subcarrierScalingFactor = subcarrierScalingFactor;
        dect2020Channel.m_channelId = channelFrequencyNumbering;
        dect2020Channel.m_centerFrequency =
            Dect2020OperatingBand::CalculateCenterFrequency(bandNumber, channelFrequencyNumbering);

        for (uint32_t slot = 0; slot < slotsPerFrame; slot++)
        {
            Slot slotObj;
            slotObj.slotId = slot;

            for (uint32_t ss = 0; ss < numSubslotsPerSlot; ss++)
            {
                Subslot subslot;
                subslot.subslotId = ss;
                subslot.status = SubslotStatus::FREE;
                subslot.rssi = 0.0;
                slotObj.subslots.push_back(subslot);
            }

            dect2020Channel.m_slots.push_back(slotObj);
        }

        NS_LOG_INFO("Channel " << channelFrequencyNumbering << " frequency "
                               << dect2020Channel.m_centerFrequency);
        m_channels.push_back(dect2020Channel);
    }
}

void
Dect2020Phy::StartFrameTimer()
{
    NS_LOG_INFO("StartFrameTimer()");

    uint64_t currentTime = Simulator::Now().GetMicroSeconds();
    uint32_t slotDuration = 416.67;    // 0,41667 ms in µs
    uint64_t frameDuration = 10000000; // 10 ms in µs

    for (uint32_t slot = 0; slot < 24; slot++)
    {

        double slotStartTime = currentTime + slot * slotDuration;
        NS_LOG_INFO("Schedule::ProcessSlot --> slotStartTime = " << slotStartTime);
        Simulator::Schedule(MicroSeconds(slotStartTime),
                            &Dect2020Phy::ProcessSlot,
                            this,
                            slot,
                            slotStartTime);
    }
    NS_LOG_INFO("Schedule::StartFrameTimer --> currentTime + frameDuration = " << currentTime + frameDuration);
    Simulator::Schedule(MicroSeconds(currentTime + frameDuration),
                        &Dect2020Phy::StartFrameTimer,
                        this);
}

void
Dect2020Phy::ProcessSlot(uint32_t slot, uint64_t slotStartTime)
{
    m_currentSlot = slot;

    NS_LOG_INFO("Processing Slot " << slot);

    // TODO: Klären, wie mit unterschiedlichen subcarrier scaling Factors umgegangen wird
    // bzw. wo er definiert wird
    uint8_t subcarrierScalingFactor = 1;
    uint32_t numSubslotsPerSlot = (subcarrierScalingFactor == 1)   ? 2
                                  : (subcarrierScalingFactor == 2) ? 4
                                  : (subcarrierScalingFactor == 4) ? 8
                                                                   : 16;

    uint32_t subslotDuration = 416.67 / numSubslotsPerSlot; // Subslot Duration in µs

    for (uint32_t subslot = 0; subslot < numSubslotsPerSlot; subslot++)
    {
        Simulator::Schedule(MicroSeconds(slotStartTime + subslot * subslotDuration),
                            &Dect2020Phy::ProcessSubslot,
                            this,
                            slot,
                            subslot);
    }
}

void
Dect2020Phy::ProcessSubslot(uint32_t slot, uint32_t subslot)
{
    m_currenSubslot = subslot;

    NS_LOG_INFO("Processing Subslot " << subslot << " in Slot " << slot);
}

Slot*
Dect2020Phy::GetCurrentSlot(uint32_t channelId) const
{
    NS_LOG_INFO("DEBUG: GetCurrentSlot() channelId = " << channelId);

    for(Dect2020Channel& channel : m_channels)
    {
        if(channel.m_channelId == channelId)
        {
            for(Slot& slot : channel.m_slots)
            {
                if(slot.slotId == m_currentSlot)
                {
                    return &slot;
                }
            }
        }
    }

    return nullptr;
}

Subslot*
Dect2020Phy::GetCurrentSubslot(uint32_t channelId) const
{
    NS_LOG_INFO("DEBUG: GetCurrentSubslot() channelId = " << channelId);

    for(Dect2020Channel& channel : m_channels)
    {
        if(channel.m_channelId == channelId)
        {
            for(Slot& slot : channel.m_slots)
            {
                for(Subslot& subslot : slot.subslots)
                {
                    if(subslot.subslotId == m_currenSubslot)
                    {
                        return &subslot;
                    }
                }
            }   
        }
    }

    return nullptr;
}

} // namespace ns3
