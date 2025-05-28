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

    uint32_t seed = this->m_mac->GetLongRadioDeviceId();
    std::srand(seed);
    uint16_t randomChannelId = 1657 + (std::rand() % 21);
    m_mac->SetCurrentChannelId(randomChannelId);

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
Dect2020Phy::Send(Ptr<Packet> packet, Dect2020PHYControlFieldType1 physicalHeader)
{
    NS_ASSERT_MSG(packet, "Packet is null");
    NS_ASSERT_MSG(this->m_device, "m_device is null");
    NS_LOG_FUNCTION(this << packet);

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Phy::Send(): Von MAC Layer empfangenes Paket mit UID "
    //             << packet->GetUid() << " und Größe " << packet->GetSize() << " Bytes von 0x"
    //             << std::hex << this->m_mac->GetLongRadioDeviceId());

    Ptr<Dect2020SpectrumSignalParameters> params = Create<Dect2020SpectrumSignalParameters>();

    params->txPhy = this;
    params->txPacket = packet->Copy();
    params->txPacket->AddHeader(physicalHeader);
    params->m_currentChannelId = this->m_mac->m_currentChannelId;

    Time duration = Time(NanoSeconds(CalculateTxDurationNs(physicalHeader)));
    params->duration = duration;

    // Get the band number from the current channel ID
    uint8_t bandId = Dect2020ChannelManager::GetBandNumber(m_mac->m_currentChannelId);

    // The following PSD Object is currently not used in this implementation.
    // Use Dect2020SpectrumModelManager::GetRssiDbm instead.

    Ptr<const SpectrumModel> specModel = Dect2020ChannelManager::GetSpectrumModel(bandId);
    Ptr<SpectrumValue> psd = Create<SpectrumValue>(specModel);

    // Get the channelIndex --> this is the index of the channel used for psd
    uint16_t channelIndex = this->m_mac->m_currentChannelId -
                            Dect2020ChannelManager::GetFirstValidChannelNumber(bandId);

    // Set the PSD value for the current channel
    Ptr<Dect2020NetDevice> dectNetDevice = DynamicCast<Dect2020NetDevice>(this->m_device);
    double txPowerWatt = Dect2020ChannelManager::DbmToW(dectNetDevice->m_txPowerDbm);
    Dect2020ChannelManager::AddSpectrumPowerToChannel(this->m_mac->m_clusterChannelId, txPowerWatt);

    for (uint32_t i = 0; i < psd->GetSpectrumModel()->GetNumBands(); ++i)
    {
        (*psd)[i] = 0.0; // Initialize all channels to 0.0
    }

    (*psd)[channelIndex] = txPowerWatt; // Set the power for the current channel
    params->psd = psd;

    // The rssiHoldTime keeps transmit power in the channel slightly longer after transmission ends.
    // This ensures that overlapping transmissions can still be detected as collisions via RSSI
    // checks.
    Time rssiHoldTime = MicroSeconds(100);
    // Remove the PSD value after the transmission
    Simulator::Schedule(duration + rssiHoldTime,
                        &Dect2020ChannelManager::RemoveSpectrumPowerFromChannel,
                        this->m_mac->m_clusterChannelId,
                        txPowerWatt);

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Phy::Send() Packet with " << params->txPacket->GetUid()
                << " and duration " << duration.GetMicroSeconds());
    // Start the transmission
    Simulator::Schedule(duration, &ns3::SpectrumChannel::StartTx, m_channel, params);

    // Trace-Aufruf
    m_phyTxBeginTrace(packet);
}

void
Dect2020Phy::SetReceiveCallback(Callback<void, Ptr<Packet>, double> cb)
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
    Ptr<Dect2020SpectrumSignalParameters> dectParams =
        DynamicCast<Dect2020SpectrumSignalParameters>(params);

    uint16_t channelIndex =
        dectParams->m_currentChannelId -
        Dect2020ChannelManager::GetFirstValidChannelNumber(
            Dect2020ChannelManager::GetBandNumber(dectParams->m_currentChannelId));
    double rssiPacket = (*params->psd)[channelIndex];
    double rssiPacketDbm = Dect2020ChannelManager::WToDbm(rssiPacket);

    // Calculate the minimum Rx sensitivity in dBm
    Ptr<Dect2020NetDevice> dectNetDevice = DynamicCast<Dect2020NetDevice>(this->m_device);
    // For Minimum RX Sensitivity see ETSI 103636 02 Table 7.2-1 --> Band 1, Bandwith 1,728 MHz == -99,7
    double minRxSensitivityDbm = -99.7 + this->m_mac->GetRxGainFromIndex(dectNetDevice->m_rxGain);
    if (dectParams->m_currentChannelId != this->m_mac->m_currentChannelId)
    {
        // abort Rx if the channel is not the same
        return;
    }

    if(rssiPacketDbm < minRxSensitivityDbm)
    {
        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Phy::StartRx() 0x" << std::hex << this->m_mac->GetLongRadioDeviceId() << std::dec << "received a Packet with UID " << dectParams->txPacket->GetUid()
                    << " received with RSSI: " << rssiPacketDbm << " dBm on channel "
                    << dectParams->m_currentChannelId << ". Packet dropped due to low RSSI ");
        return;
    }

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds() << ": Dect2020Phy::StartRx: 0x" << std::hex << this->m_mac->GetLongRadioDeviceId() << std::dec
                << " received a Packet with UID " << dectParams->txPacket->GetUid());
    // NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
    //             << ": Dect2020Phy::StartRx() Packet with UID " << dectParams->txPacket->GetUid()
    //             << " and RSSI: " << rssiPacketDbm << " dBm on channel "
    //             << dectParams->m_currentChannelId);

    auto rssiChannelDbm = Dect2020ChannelManager::GetRssiDbm(dectParams->m_currentChannelId);


    if (rssiChannelDbm > 11.5 && rssiChannelDbm < 13.1)
    {
        Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable>();
        double rand = randVar->GetValue(0.0, 1.0);
        if (rand > 0.5)
        {
            NS_LOG_WARN(Simulator::Now().GetMicroSeconds()
                        << ": Collision detected. Device 0x" << std::hex << this->m_mac->GetLongRadioDeviceId() << std::dec << " dropped the Packet with UID " << dectParams->txPacket->GetUid());
            return;
        }
    }
    else if(rssiChannelDbm > 13.1)
    {
        NS_LOG_WARN(Simulator::Now().GetMicroSeconds()
                    << ": Collision detected. Device 0x" << std::hex << this->m_mac->GetLongRadioDeviceId() << std::dec << " dropped the Packet with UID " << dectParams->txPacket->GetUid());
        return;
    }

    m_receiveCallback(dectParams->txPacket, rssiPacketDbm);
}

void
Dect2020Phy::StartFrameTimer()
{
    // NS_LOG_INFO("StartFrameTimer() called at time " << Simulator::Now().GetMicroSeconds());

    // auto currentTime = Simulator::Now().GetMicroSeconds();
    double slotDuration = 416666;   // 0,41667 ms in ns
    uint16_t frameDuration = 10000; // 10 ms in µs
    m_frameStartTime = Simulator::Now();

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
    if (slot == 0)
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

    double subslotDuration = 416666 / numSubslotsPerSlot; // Subslot Duration in µs

    for (uint32_t subslot = 0; subslot < numSubslotsPerSlot; subslot++)
    {
        Simulator::Schedule(NanoSeconds(subslot * subslotDuration),
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
Dect2020Phy::CalculateTxDurationNs(Dect2020PHYControlFieldType1 physicalHeaderField)
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

// Time
// Dect2020Phy::GetAbsoluteSubslotTime(uint8_t targetSfn, uint8_t slot, uint8_t subslot) const
// {
//     // uint32_t slotsPerFrame = 24;
//     uint32_t subslotsPerSlot = this->m_mac->GetSubslotsPerSlot();
//     // uint32_t subslotsPerFrame = slotsPerFrame * subslotsPerSlot;

//     Time frameDuration = MicroSeconds(10000);                     // 10 ms in µs
//     Time slotDuration = NanoSeconds(416666);                      // 0,41667 ms in ns
//     Time subslotDuration = NanoSeconds(416666 / subslotsPerSlot); // Subslot Duration in ns

//     // uint8_t deltaFrames = (targetSfn - m_currentSfn) % 256;
//     int32_t deltaFrames = static_cast<int32_t>(targetSfn) - static_cast<int32_t>(m_currentSfn);

//     if (deltaFrames < 0)
//     {
//         // If the target SFN is less than the current SFN, we need to wrap around.
//         // We want to calculate the offset time of the current Frame.
//         deltaFrames = 0;
//     }

//     Time offset = deltaFrames * frameDuration + slot * slotDuration + subslot * subslotDuration;

//     // DEBUG
//     // auto fff = offset.GetNanoSeconds();
//     // auto fst = m_frameStartTime.GetNanoSeconds();
//     // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
//     //             << ": DEBUG: GetAbsoluteSubslotTime() targetSfn = " <<
//     //             static_cast<int>(targetSfn)
//     //             << ", m_currentSfn = " << static_cast<int>(m_currentSfn) << ", slot = "
//     //             << static_cast<int>(slot) << ", subslot = " << static_cast<int>(subslot)
//     //             << ", m_frameStartTime + offset = " << fst + fff);
//     // ###############################

//     NS_LOG_INFO("offset = " << offset.GetMicroSeconds() << std::endl
//                             << "m_frameStartTime = " << m_frameStartTime.GetMicroSeconds()
//                             << std::endl
//                             << " Simulator::Now().GetMicroSeconds() = "
//                             << Simulator::Now().GetMicroSeconds());

//     return offset + m_frameStartTime;
// }

Time
Dect2020Phy::GetTimeToNextAbsoluteSubslot(uint8_t targetSfn,
                                          uint8_t targetSlot,
                                          uint8_t targetSubslot) const
{
    // uint8_t currentSfn = m_currentSfn;
    uint8_t currentSlot = m_currentSlot;
    uint8_t currentSubslot = m_currentSubslot;

    // Calculate the absolute subslot index for the current and target subslot
    int currentAbsoluteSubslotIndex = GetAbsoluteSubslotIndex(currentSlot, currentSubslot);
    int targetAbsoluteSubslotIndex = GetAbsoluteSubslotIndex(targetSlot, targetSubslot);

    // Calculate the difference in absolute subslot indices
    int subslotDifference = targetAbsoluteSubslotIndex - currentAbsoluteSubslotIndex;

    // If the target subslot is in the next frame, adjust the difference
    if (subslotDifference < 0)
    {
        // If the target subslot is in the next frame, add the total number of subslots in a frame
        uint32_t subslotsPerFrame = this->m_mac->GetSubslotsPerSlot() * 24; // 24 slots per frame
        subslotDifference += subslotsPerFrame;
    }

    // Calculate the time until the target subslot
    Time subslotDuration =
        NanoSeconds(416666 / this->m_mac->GetSubslotsPerSlot()); // Subslot Duration in ns
    Time timeUntilNextSubslot = subslotDifference * subslotDuration;

    return timeUntilNextSubslot;
}

int
Dect2020Phy::GetAbsoluteSubslotIndex(uint8_t slot, uint8_t subslot) const
{
    // Calculate the absolute subslot index
    uint8_t subslotsPerSlot = this->m_mac->GetSubslotsPerSlot();
    int absoluteSubslotIndex = (slot * subslotsPerSlot) + subslot;

    return absoluteSubslotIndex;
}

} // namespace ns3
