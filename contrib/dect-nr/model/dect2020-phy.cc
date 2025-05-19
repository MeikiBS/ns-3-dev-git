/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-phy.h"

#include "dect2020-channel-manager.h"
#include "dect2020-channel.h"
#include "dect2020-mac.h"
#include "dect2020-net-device.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

#include <algorithm>
#include <iomanip>

// Kann später wieder weg, nur DEBUG:
#include <cstdlib>
#include <ctime>

NS_LOG_COMPONENT_DEFINE("Dect2020Phy");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Phy);

// bool Dect2020Phy::m_isFrameTimerRunning = false;

const std::vector<uint16_t> Dect2020Phy::m_singleSlotSingleStreamTransportBlockSizesMu1Beta1{
    136,  // MCS 0
    296,  // MCS 1
    456,  // MCS 2
    616,  // MCS 3
    936,  // MCS 4
    1256, // MCS 5
    1416, // MCS 6
    1576, // MCS 7
    1896, // MCS 8
    2040, // MCS 9
    2296, // MCS 10
    2552  // MCS 11
};

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
      m_currentSubslot(0)
{
    // // Set a start channel
    // Ptr<Dect2020NetDevice> device = DynamicCast<Dect2020NetDevice>(this->m_device);
    // Ptr<Dect2020Channel> startChannel =
    // Dect2020ChannelManager::GetValidChannels(device->GetBandNumber()).front(); m_dect2020Channel
    // = startChannel;

    // if (!m_isFrameTimerRunning)
    // {
    //     m_isFrameTimerRunning = true;
    //     StartFrameTimer();
    // }
    NS_LOG_FUNCTION(this);
}

Dect2020Phy::~Dect2020Phy()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020Phy::Start()
{
    // Set a start channel
    Ptr<Dect2020NetDevice> device = DynamicCast<Dect2020NetDevice>(this->m_device);
    Ptr<Dect2020Channel> startChannel =
        Dect2020ChannelManager::GetValidChannels(device->GetBandNumber()).front();
    m_dect2020Channel = startChannel;
    // this->m_mac->SetCurrentChannelId(m_dect2020Channel->m_channelId);
    
    
    uint32_t seed = this->m_mac->GetLongRadioDeviceId();
    std::srand(seed);
    uint16_t randomChannelId = 1657 + (std::rand() % 21);
    m_mac->SetCurrentChannelId(randomChannelId);

    NS_LOG_INFO("Set Random Channel ID: " << m_mac->m_currentChannelId);
    if (!m_isFrameTimerRunning)
    {
        m_isFrameTimerRunning = true;
        StartFrameTimer();
    }
}

void
Dect2020Phy::SetMac(Ptr<Dect2020Mac> mac)
{
    NS_LOG_FUNCTION(this << mac);
    m_mac = mac;
}

void
Dect2020Phy::SetDevice(Ptr<NetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
}

void
Dect2020Phy::SetChannel(Ptr<SpectrumChannel> channel)
{
    NS_LOG_FUNCTION(this << channel);
    m_channel = channel;

    // TODO: Funktion prüfen.
    // Add the Dect2020Phy to the Spectrum Channel, so it can receive Packets
    if (m_channel)
    {
        m_channel->AddRx(this);
    }
}

Ptr<SpectrumChannel>
Dect2020Phy::GetChannel(void) const
{
    return m_channel;
}

Ptr<NetDevice>
Dect2020Phy::GetDevice(void) const
{
    return m_device;
}

void
Dect2020Phy::Send(Ptr<Packet> packet, Dect2020PhysicalHeaderField physicalHeader)
{
    NS_ASSERT_MSG(packet, "Packet is null");
    NS_ASSERT_MSG(this->m_device, "m_device is null");

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Phy::Send(): Von MAC Layer empfangenes Paket mit UID "
    //             << packet->GetUid() << " und Größe " << packet->GetSize() << " Bytes von 0x"
    //             << std::hex << this->m_mac->GetLongRadioDeviceId());

    NS_LOG_FUNCTION(this << packet);

    Ptr<Dect2020SpectrumSignalParameters> params = Create<Dect2020SpectrumSignalParameters>();

    params->txPhy = this;
    params->txPacket = packet->Copy();
    params->txPacket->AddHeader(physicalHeader);
    params->m_currentChannelId = this->m_mac->m_currentChannelId;

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Phy::Send(): Physical Header hinzugefügt bei Paket mit UID "
    //             << params->txPacket->GetUid() << " und neuer Größe " << params->txPacket->GetSize()
    //             << " Bytes von 0x" << std::hex << this->m_mac->GetLongRadioDeviceId());

    Time duration = Time(NanoSeconds(CalculateTxDurationNs(physicalHeader)));
    params->duration = duration;

    // Get the band number from the current channel ID
    uint8_t bandId = Dect2020ChannelManager::GetBandNumber(m_mac->m_currentChannelId);

    // The following PSD Object is currently not used in this implementation.
    // Use Dect2020SpectrumModelManager::GetRssiDbm instead.

    Ptr<const SpectrumModel> specModel = Dect2020ChannelManager::GetSpectrumModel(bandId);
    Ptr<SpectrumValue> psd = Create<SpectrumValue>(specModel);
    params->psd = psd;

    // Set the PSD value for the current channel
    double power = Dect2020ChannelManager::DbmToW(23);
    Dect2020ChannelManager::AddSpectrumPowerToChannel(this->m_mac->m_clusterChannelId, power);

    // Remove the PSD value after the transmission
    Simulator::Schedule(duration,
                        &Dect2020ChannelManager::RemoveSpectrumPowerFromChannel,
                        this->m_mac->m_clusterChannelId,
                        power);

    // Start the transmission
    Simulator::Schedule(duration, &ns3::SpectrumChannel::StartTx, m_channel, params);

    // Trace-Aufruf
    m_phyTxBeginTrace(packet);
}

void
Dect2020Phy::Receive(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    m_phyRxTrace(packet);

    if (m_mac)
    {
        m_mac->ReceiveFromPhy(packet);
    }

    // Empfangsverzögerung
    // Simulator::Schedule(m_rxDelay, &Dect2020Phy::ReceiveDelayed, this, packet);
}

void
Dect2020Phy::ReceiveDelayed(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // Trace-Aufruf
    m_phyRxTrace(packet);

    // Weiterleitung an die MAC-Schicht
    m_mac->ReceiveFromPhy(packet);
}

void
Dect2020Phy::SetReceiveCallback(Callback<void, Ptr<Packet>> cb)
{
    m_receiveCallback = cb;
}

uint16_t
Dect2020Phy::GetCurrentAbsoluteSubslot() const
{
    return m_currentAbsoluteSubslot;
}

void
Dect2020Phy::SetMobility(Ptr<MobilityModel> m)
{
    m_mobilityModel = m;
}

Ptr<MobilityModel>
Dect2020Phy::GetMobility() const
{
    return m_mobilityModel;
}

Ptr<const SpectrumModel>
Dect2020Phy::GetRxSpectrumModel() const
{
    return m_spectrumModel;
}

Ptr<Object>
Dect2020Phy::GetAntenna() const
{
    return m_antenna;
}

void
Dect2020Phy::StartRx(Ptr<SpectrumSignalParameters> params)
{
    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Phy: StartRx called from Device 0x" << std::hex
    //             << this->m_mac->GetLongRadioDeviceId());

    Ptr<Dect2020SpectrumSignalParameters> dectParams =
        DynamicCast<Dect2020SpectrumSignalParameters>(params);

    // NS_LOG_INFO("StartRx() aufgerufen von 0x" << std::hex << this->m_mac->GetShortRadioDeviceId()
    // << std::dec << ". RD Channel: " << this->m_mac->m_clusterChannelId
    //             << " und Params Channel: " << dectParams->m_currentChannelId);

    // if (dectParams->m_currentChannelId == this->m_mac->m_currentChannelId)
    // {
    //     NS_LOG_INFO(Simulator::Now().GetMilliSeconds() << ": Message empfangen auf channel " <<
    //                 dectParams->m_currentChannelId << " mit UID " <<
    //                 dectParams->txPacket->GetUid());
    //     return;
    // }

    if (dectParams->m_currentChannelId != this->m_mac->m_currentChannelId)
    {
        // abort Rx if the channel is not the same
        return;
    }

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Dect2020Phy::StartRx(): Device 0x" << std::hex
                << this->m_mac->GetLongRadioDeviceId() << std::dec << " empfängt Paket mit UID "
                << dectParams->txPacket->GetUid() << " und Größe "
                << dectParams->txPacket->GetSize() << " Bytes vom Kanal.");

    // WARUM??
    // double power = Dect2020ChannelManager::GetRssiDbm(this->m_mac->m_currentChannelId);
    // Subslot* subslot = GetCurrentSubslot(this->m_mac->m_currentChannelId);
    // subslot->rssi = power;

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Phy::StartRx(): Current global PSD: " << power);

    m_receiveCallback(dectParams->txPacket);
}

// void
// Dect2020Phy::InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor)
// {
//     const uint32_t slotsPerFrame = 24;    // #ETSI 103 636-3 V1.5.1, Section 4.4

//     Dect2020OperatingBand operatingBand;
//     BandParameters bandParams = operatingBand.GetBandParameters(bandNumber);

//     uint32_t numChannels = (bandParams.nEnd - bandParams.nStart) + 1; // Number of Channels

//     if (!(subcarrierScalingFactor == 1 || subcarrierScalingFactor == 2 ||
//           subcarrierScalingFactor == 4 || subcarrierScalingFactor == 8))
//     {
//         NS_LOG_INFO("Subcarrier scaling factor invalid (not yet implemented), set to 1");
//         subcarrierScalingFactor = 1;
//     }
//     uint32_t numSubslotsPerSlot = (subcarrierScalingFactor == 1)   ? 2
//                                   : (subcarrierScalingFactor == 2) ? 4
//                                   : (subcarrierScalingFactor == 4) ? 8
//                                                                    : 16;
//     const double slotDurationNs = 10000000 / slotsPerFrame; // 0,41667 ms in ns
//     double subslotDurationNs = slotDurationNs / numSubslotsPerSlot; // Subslot Duration in ns

//     this->m_channels.clear();
//     for (uint32_t ch = 0; ch < numChannels; ch++)
//     {
//         int channelFrequencyNumbering = bandParams.nStart + ch;

//         Dect2020Channel dect2020Channel;
//         dect2020Channel.m_subcarrierScalingFactor = subcarrierScalingFactor;
//         dect2020Channel.m_channelId = channelFrequencyNumbering;
//         dect2020Channel.m_centerFrequency =
//             Dect2020OperatingBand::CalculateCenterFrequency(bandNumber,
//             channelFrequencyNumbering);

//         for (uint32_t slot = 0; slot < slotsPerFrame; slot++)
//         {
//             Slot slotObj;
//             slotObj.slotId = slot;

//             for (uint32_t ss = 0; ss < numSubslotsPerSlot; ss++)
//             {
//                 Subslot subslot;
//                 subslot.subslotId = ss;
//                 subslot.status = SubslotStatus::FREE;
//                 subslot.rssi = 0.0;
//                 subslot.subslotDurationNs = subslotDurationNs;
//                 slotObj.subslots.push_back(subslot);
//             }

//             // dect2020Channel.m_slots.push_back(slotObj);
//             dect2020Channel.AddSlot(slotObj); // Add the current Slot to the Channel
//         }

//         // NS_LOG_INFO("Channel " << channelFrequencyNumbering << " frequency "
//         //                        << dect2020Channel.m_centerFrequency);
//         m_channels.push_back(dect2020Channel);
//     }
// }

void
Dect2020Phy::StartFrameTimer()
{
    // NS_LOG_INFO("StartFrameTimer() called at time " << Simulator::Now().GetMicroSeconds());

    // auto currentTime = Simulator::Now().GetMicroSeconds();
    double slotDuration = 416666;   // 0,41667 ms in ns
    uint16_t frameDuration = 10000; // 10 ms in µs

    for (uint32_t slot = 0; slot < 24; slot++)
    {
        double slotStartTime = slot * slotDuration;
        // NS_LOG_INFO("Schedule::ProcessSlot Slot " << slot << " at slotStartTime " << std::fixed
        //                                           << std::setprecision(2) << slotStartTime);
        Simulator::Schedule(NanoSeconds(slotStartTime),
                            &Dect2020Phy::ProcessSlot,
                            this,
                            slot,
                            slotStartTime);
    }
    // NS_LOG_INFO("Schedule::StartFrameTimer --> currentTime + frameDuration = "
    //             << std::fixed << currentTime + frameDuration);
    Simulator::Schedule(MicroSeconds(frameDuration), &Dect2020Phy::StartFrameTimer, this);
}

void
Dect2020Phy::ProcessSlot(uint32_t slot, double slotStartTime)
{
    if(slot == 0)
    {
        m_currentSfn = (m_currentSfn + 1) % 256;
    }
    m_currentSlot = slot;

    // NS_LOG_INFO("Processing Slot " << slot << " at time " << std::fixed
    //                                << Simulator::Now().GetMicroSeconds());

    uint8_t subcarrierScalingFactor = 1;
    uint32_t numSubslotsPerSlot = (subcarrierScalingFactor == 1)   ? 2
                                  : (subcarrierScalingFactor == 2) ? 4
                                  : (subcarrierScalingFactor == 4) ? 8
                                                                   : 16;

    double subslotDuration = 416667 / numSubslotsPerSlot; // Subslot Duration in µs

    for (uint32_t subslot = 0; subslot < numSubslotsPerSlot; subslot++)
    {
        // NS_LOG_INFO("Schedule::ProcessSubslot Subslot "
        //             << subslot << " in Slot " << slot << " at Time " << std::fixed
        //             << (slotStartTime + subslot * subslotDuration) << " and subslotDuration "
        //             << subslotDuration);
        Simulator::Schedule(NanoSeconds(slotStartTime + (subslot * subslotDuration)),
                            &Dect2020Phy::ProcessSubslot,
                            this,
                            slot,
                            subslot);
    }
}

void
Dect2020Phy::ProcessSubslot(uint32_t slotId, uint32_t subslotId)
{
    m_currentSubslot = subslotId;
    m_currentAbsoluteSubslot = (slotId * 2) + subslotId;

    // Reset the RSSI of the current Subslot
    Subslot* subslot = GetCurrentSubslot(this->m_mac->m_clusterChannelId);
    (*subslot).rssi = 0;

    // NS_LOG_INFO("Processing Subslot " << std::fixed << subslot << " in Slot " << slot << " at
    // time "
    //                                   << std::fixed << Simulator::Now().GetMicroSeconds());
}

Slot*
Dect2020Phy::GetCurrentSlot(uint32_t channelId)
{
    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": DEBUG: GetCurrentSlot() channelId = " << channelId);

    for (Slot& slot : m_dect2020Channel->m_slots)
    {
        if (slot.slotId == m_currentSlot)
        {
            return &slot;
        }
    }

    return nullptr;
}

Subslot*
Dect2020Phy::GetCurrentSubslot(uint32_t channelId)
{
    // NS_LOG_INFO("DEBUG: GetCurrentSubslot() channelId = " << channelId);

    for (Slot& slot : m_dect2020Channel->m_slots)
    {
        if (slot.slotId == m_currentSlot)
        {
            for (Subslot& subslot : slot.subslots)
            {
                if (subslot.subslotId == m_currentSubslot)
                {
                    // NS_LOG_INFO(Simulator::Now().GetNanoSeconds() << ": DEBUG: Subslot "
                    //             << subslot.subslotId << " in Slot " << slot.slotId);
                    return &subslot;
                }
            }
        }
    }

    return nullptr;
}

uint16_t
Dect2020Phy::GetMcsTransportBlockSize(uint8_t mu, uint8_t beta, uint8_t mcsIndex)
{
    if (mu == 1 && beta == 1)
    {
        if (mcsIndex >= m_singleSlotSingleStreamTransportBlockSizesMu1Beta1.size())
        {
            NS_LOG_WARN("Invalid MCS index for (mu = 1, beta = 1)");
            return 0;
        }
        return m_singleSlotSingleStreamTransportBlockSizesMu1Beta1[mcsIndex];
    }

    NS_LOG_WARN("Unsupported (mu,beta) combination in GetMCSTransportBlockSize");
    return 0;
}

double
Dect2020Phy::CalculateTxDurationNs(Dect2020PhysicalHeaderField physicalHeaderField)
{
    if (physicalHeaderField.GetPacketLengthType() == 0) // Packet length is given in subslots
    {
        // TBD
    }
    else if (physicalHeaderField.GetPacketLengthType() == 1) // Packet length is given in slots
    {
        return physicalHeaderField.GetPacketLength() * 416670; // 416.67 ns per slot
    }
    else
    {
        NS_LOG_WARN("Invalid packet length type");
        return 0;
    }

    return 0;
}

} // namespace ns3
