/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-mac.h"

#include "dect2020-channel-manager.h"
#include "dect2020-mac-common-header.h"
#include "dect2020-mac-header-type.h"
#include "dect2020-mac-messages.h"
#include "dect2020-net-device.h"
#include "dect2020-phy.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

#include <iomanip> // Für std::setw und std::setfill

NS_LOG_COMPONENT_DEFINE("Dect2020Mac");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Mac);

TypeId
Dect2020Mac::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020Mac")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020Mac>();
    return tid;
}

Dect2020Mac::Dect2020Mac()
{
    NS_LOG_FUNCTION(this);

    // TODO: Channel sucht sich ein RD selbst aus
    m_operatingChannelId = 1658;

    InitializeDevice(); // Initialize the Device
}

Dect2020Mac::~Dect2020Mac()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020Mac::SetNetDevice(Ptr<Dect2020NetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
    m_address = Mac48Address::ConvertFrom(device->GetAddress());
}

void
Dect2020Mac::SetPhy(Ptr<Dect2020Phy> phy)
{
    NS_LOG_FUNCTION(this << phy);
    m_phy = phy;

    m_phy->SetReceiveCallback(MakeCallback(&Dect2020Mac::ReceiveFromPhy, this));
}

void
Dect2020Mac::Send(Ptr<Packet> packet, const Address& dest, Dect2020PacketType type)
{
    NS_LOG_FUNCTION(this << packet << dest << type);

    // // Hier können MAC-Header hinzugefügt werden
    // if (type == BEACON)
    // {
    //     Dect2020BeaconHeader beaconHeader;
    //     beaconHeader.SetNetworkId(this->GetNetworkId());
    //     beaconHeader.SetTransmitterAddress(this->GetLongRadioDeviceId());
    // }

    // Senden des Pakets über die PHY-Schicht
    // m_phy->Send(packet);

    // Trace-Aufruf
    m_txPacketTrace(packet);
}

void
Dect2020Mac::ReceiveFromPhy(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Mac::ReceiveFromPhy() aufgerufen von 0x" << std::hex
    //             << this->GetLongRadioDeviceId());

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Dect2020Mac::ReceiveFromPhy(): Device 0x" << std::hex
                << this->GetLongRadioDeviceId() << std::dec << " hat Paket mit UID "
                << packet->GetUid() << " mit der Größe von " << packet->GetSize()
                << " Bytes empfangen." << std::endl);

    Dect2020PhysicalHeaderField physicalHeaderField;
    packet->RemoveHeader(physicalHeaderField);
    // NS_LOG_INFO(physicalHeaderField);

    // Jetzt sicher entfernen
    Dect2020NetworkBeaconMessage beaconMessage;
    packet->RemoveHeader(beaconMessage);
    // NS_LOG_INFO(beaconMessage);

    Dect2020BeaconHeader beaconHeader;
    packet->RemoveHeader(beaconHeader);
    // NS_LOG_INFO(beaconHeader);

    Dect2020MacHeaderType macHeaderType;
    // packet->PeekHeader(macHeaderType);
    // NS_LOG_INFO(macHeaderType);
    packet->RemoveHeader(macHeaderType); // Jetzt passt die Reihenfolge
    // NS_LOG_INFO(macHeaderType);

    if (macHeaderType.GetMacHeaderTypeField() ==
        Dect2020MacHeaderType::MacHeaderTypeField::BEACON_HEADER)
    {
        NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                    << ": Dect2020Mac::ReceiveFromPhy(): Beacon with Network ID: 0x" << std::hex
                    << beaconHeader.GetNetworkId() << " received from Device 0x" << std::hex
                    << beaconHeader.GetTransmitterAddress());
    }
    else
    {
        NS_LOG_WARN(Simulator::Now().GetMilliSeconds() << ": Received unknown Message.");
    }

    // Weiterleitung des Pakets an das NetDevice
    m_device->Receive(packet);

    // Trace-Aufruf
    m_rxPacketTrace(packet);
}

Mac48Address
Dect2020Mac::GetAddress(void) const
{
    return m_address;
}

void
Dect2020Mac::Start()
{
    if (this->m_device->GetTerminationPointType() == Dect2020NetDevice::TerminationPointType::FT)
    {
        InitializeNetwork();
    }
    else if (this->m_device->GetTerminationPointType() ==
             Dect2020NetDevice::TerminationPointType::PT)
    {
        // DEBUG: Network ID auf festen Wert setzen, später mit Beacon empfangen
        // m_networkId = 123456;
        // JoinNetwork(m_networkId);

        Simulator::Schedule(MilliSeconds(100),
                            &Dect2020Mac::DiscoverNetworks,
                            this); // Start Discovery
    }
}

void
Dect2020Mac::InitializeNetwork()
{
    uint32_t networkId = GenerateValidNetworkId();
    SetNetworkId(networkId);

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ":  FT-Device " << this << " started a new Network with the Network ID: "
                << std::hex << std::setw(8) << std::setfill('0') << networkId);

    Simulator::Schedule(Seconds(1), &Dect2020Mac::OperatingChannelSelection, this);
    // Simulator::Schedule(Seconds(1), &Dect2020Mac::StartBeaconTransmission, this);
    // OperatingChannelSelection();
    // StartBeaconTransmission();
}

/**
 * \brief Initiates or continues the network discovery procedure for a PT device.
 *
 * This method simulates a periodic channel sweep by a portable termination (PT) device
 * to discover available fixed terminations (FTs) in a DECT-2020 NR network. It increases
 * the current operating channel by one and switches to that channel if it exists in the
 * current band. If the next channel is not available (e.g., end of band), the channel
 * list is wrapped and the PT starts scanning again from the beginning.
 *
 * The method reschedules itself after a fixed delay (e.g., 100 ms) until the device
 * becomes associated with an FT.
 *
 */
void
Dect2020Mac::DiscoverNetworks()
{
    std::vector<Ptr<Dect2020Channel>> channelList =
        Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());

    if (m_associatedFTNetDevice != nullptr)
    {
        return; // Device is already associated --> abort
    }

    // find the current channel
    uint32_t current = m_operatingChannelId;
    uint32_t nextChannelId = 0;

    // search if current + 1 is part of the channel list
    bool found = false;
    for (auto& ch : channelList)
    {
        if (ch->m_channelId == current + 1)
        {
            nextChannelId = ch->m_channelId;
            found = true;
            break;
        }
    }

    if (!found)
    {
        // no valid next channel -> take the first one of the band
        nextChannelId = channelList.front()->m_channelId;
    }

    SetCurrentChannelId(nextChannelId);
    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": PT-Device " << std::hex << "0x" << GetShortRadioDeviceId()
                << " is scanning channel: " << std::dec << m_operatingChannelId);

    Time t = MilliSeconds(100); // Discover Network wait time
    Simulator::Schedule(t, &Dect2020Mac::DiscoverNetworks,
                        this); // Schedule next discovery
}

void
Dect2020Mac::JoinNetwork(uint32_t networkId)
{
    SetNetworkId(networkId);

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": PT-Device joined a Network with the Network ID: " << std::hex << std::setw(8)
                << std::setfill('0') << networkId);

    SetShortRadioDeviceId(GenerateShortRadioDeviceId());
    // TODO: Unter welchen Umständen kann einem Network beigetreten/nicht beigetreten werden?
}

void
Dect2020Mac::StartNetworkBeaconSweep()
{
    std::vector<Ptr<Dect2020Channel>> channelList =
        Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());

    int16_t operatingChannel = static_cast<int16_t>(m_operatingChannelId);
    std::vector<int16_t> networkBeaconChannels;

    // find valid channels with a higher channel id than the operating channel +2, +4, ...
    for (int16_t offset = 2;; offset += 2)
    {
        int16_t candidate = operatingChannel + offset;
        if (!Dect2020ChannelManager::ChannelExists(candidate))
        {
            break;
        }
        networkBeaconChannels.push_back(candidate);
    }

    // find valid channels with a lower channel id than the operating channel -2, -4, ...
    for (int16_t offset = 2;; offset += 2)
    {
        int16_t candidate = operatingChannel - offset;
        if (!Dect2020ChannelManager::ChannelExists(candidate))
        {
            break;
        }
        networkBeaconChannels.push_back(candidate);
    }

    // Schedule the network beacon transmission on the selected channels
    Time beaconDuration = MicroSeconds(1);        // duration of the beacon transmission
    Time networkBeaconPeriod = MilliSeconds(100); // gap between each transmission
    Time base = Seconds(0);

    for (auto& channelId : networkBeaconChannels)
    {
        Simulator::Schedule(base, &Dect2020Mac::SendNetworkBeaconOnChannel, this, channelId);

        // Go back to the operating channel after beacon transmission
        Simulator::Schedule(base + beaconDuration, &Dect2020Mac::ReturnToOperatingChannel, this);

        base += networkBeaconPeriod;
    }

    // Schedule the next sweep
    Simulator::Schedule(base + beaconDuration, &Dect2020Mac::StartNetworkBeaconSweep, this);
}

void
Dect2020Mac::ReturnToOperatingChannel()
{
    SetCurrentChannelId(m_operatingChannelId);
}

void
Dect2020Mac::SendNetworkBeaconOnChannel(uint16_t channelId)
{
    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Dect2020Mac::SendNetworkBeaconOnChannel " << channelId
                << ". Aktueller operating channel: " << m_operatingChannelId);

    SetCurrentChannelId(channelId);
    Ptr<Packet> networkBeacon = BuildBeacon(false);

    m_phy->Send(networkBeacon, CreatePhysicalHeaderField(1, networkBeacon->GetSize()));
}

Ptr<Packet>
Dect2020Mac::BuildBeacon(bool isCluster)
{
    Ptr<Packet> beacon = Create<Packet>();

    Dect2020MacHeaderType macHeaderType;
    macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::BEACON_HEADER);
    beacon->AddHeader(macHeaderType);

    Dect2020BeaconHeader beaconHeader;
    beaconHeader.SetNetworkId(m_networkId);
    beaconHeader.SetTransmitterAddress(m_longRadioDeviceId);
    beacon->AddHeader(beaconHeader);

    if (isCluster)
    {
        Dect2020ClusterBeaconMessage msg;
        // TODO: füllen
        beacon->AddHeader(msg);
    }
    else
    {
        Dect2020NetworkBeaconMessage msg;
        // TODO: füllen
        beacon->AddHeader(msg);
    }

    return beacon;
}

void
Dect2020Mac::StartBeaconTransmission()
{
    StartNetworkBeaconSweep();
    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Start Beacon Transmission on channel: " << m_operatingChannelId);

    // Ptr<Packet> networkBeacon = Create<Packet>();

    // // MAC Header Type
    // Dect2020MacHeaderType macHeaderType;
    // macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::BEACON_HEADER);
    // networkBeacon->AddHeader(macHeaderType);

    // // Mac Beacon Header
    // Dect2020BeaconHeader beaconHeader;
    // beaconHeader.SetNetworkId(m_networkId);
    // beaconHeader.SetTransmitterAddress(m_longRadioDeviceId);

    // networkBeacon->AddHeader(beaconHeader);

    // // MAC Beacon Message
    // Dect2020NetworkBeaconMessage beaconMessage;
    // // TODO: beaconMessage mit Inhalt füllen

    // networkBeacon->AddHeader(beaconMessage);

    // NS_LOG_INFO("Größe des Pakets direkt nach beaconMessage: " << networkBeacon->GetSize());

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": StartBeaconTransmission() aufgerufen von 0x" << std::hex
    //             << this->GetLongRadioDeviceId() << " übergibt Paket mit der Größe " << std::dec
    //             << networkBeacon->GetSize() << " Bytes und UID " << networkBeacon->GetUid()
    //             << " an PHY.");

    // m_phy->Send(networkBeacon, CreatePhysicalHeaderField(1, networkBeacon->GetSize())); //

    // // NS_LOG_INFO("Network Beacon gesendet von Gerät 0x"
    // //             << std::hex  << this->GetLongRadioDeviceId());
    // // NS_LOG_INFO("MAC Header Type: " << macHeaderType.GetMacHeaderTypeField());

    // Simulator::Schedule(MilliSeconds(beaconMessage.GetNetworkBeaconPeriodTime()),
    //                     &Dect2020Mac::StartBeaconTransmission,
    //                     this);
}

void
Dect2020Mac::StartSubslotScan(uint32_t channelId,
                              uint32_t numSubslots,
                              std::function<void(const ChannelEvaluation&)> onComplete)
{
    auto context = std::make_shared<SubslotScanContext>();
    context->channelId = channelId;
    context->evaluation.channelId = channelId;
    context->onComplete = onComplete;
    context->subslotCount = 0;

    ScheduleNextSubslotMeasurement(context, numSubslots);
}

void
Dect2020Mac::ScheduleNextSubslotMeasurement(std::shared_ptr<SubslotScanContext> context,
                                            uint32_t numSubslots)
{
    Subslot* subslot = m_phy->GetCurrentSubslot(context->channelId);

    double rssi = Dect2020ChannelManager::GetRssiDbm(context->channelId);

    if (rssi <= RSSI_THRESHOLD_MIN)
    {
        context->evaluation.free++;
    }
    else if (rssi <= RSSI_THRESHOLD_MAX)
    {
        context->evaluation.possible++;
    }
    else
    {
        context->evaluation.busy++;
    }

    context->subslotCount++;

    if (context->subslotCount < numSubslots)
    {
        // Schedule the next measurement
        Simulator::Schedule(NanoSeconds(subslot->subslotDurationNs),
                            &Dect2020Mac::ScheduleNextSubslotMeasurement,
                            this,
                            context,
                            numSubslots);
    }
    else
    {
        // Call the completion callback with the evaluation
        context->onComplete(context->evaluation);
    }
}

void
Dect2020Mac::OperatingChannelSelection()
{
    m_scanEvaluations.clear();
    m_completedScans = 0;

    int numSubslots = 48; // TODO: Get number of Subslots from configuration

    double subslotDurationNs = 208333.0;
    double totalScanTimeNs = subslotDurationNs * numSubslots;

    uint32_t channelOffset = 0;
    // for (auto& channel : m_phy->m_channels)
    auto validChannels = Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());
    uint16_t numOfValidChannels = validChannels.size();
    for (auto& channel : validChannels)
    {
        Time delay = NanoSeconds(channelOffset * totalScanTimeNs);

        Simulator::Schedule(delay,
                            &Dect2020Mac::StartSubslotScan,
                            this,
                            channel->m_channelId,
                            48,
                            [this, numOfValidChannels](const ChannelEvaluation& eval) {
                                m_scanEvaluations[eval.channelId] = eval;
                                m_completedScans++;

                                if (m_completedScans == numOfValidChannels)
                                {
                                    EvaluateAllChannels();
                                }
                            });

        channelOffset++;
    }
}

void
Dect2020Mac::EvaluateAllChannels()
{
    std::vector<ChannelEvaluation> evaluations;

    for (const auto& [channelId, eval] : m_scanEvaluations)
    {
        evaluations.push_back(eval);
    }

    // 1. Search 100 % free channel
    for (const auto& e : evaluations)
    {
        uint32_t total = e.Total();

        if (e.free == total)
        {
            m_operatingChannelId = e.channelId;
            NS_LOG_INFO("Selected COMPLETELY FREE channel: " << e.channelId);

            // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
            Simulator::Schedule(Seconds(SCAN_STATUS_VALID),
                                &Dect2020Mac::OperatingChannelSelection,
                                this);
            // Start the beacon transmission
            StartBeaconTransmission();
            return;
        }
    }

    // 2. Search channel with suitable conditions
    for (const auto& e : evaluations)
    {
        uint32_t total = e.Total();
        uint32_t suitable = e.free + e.possible;

        if (suitable >= static_cast<uint32_t>(total * SCAN_SUITABLE))
        {
            m_operatingChannelId = e.channelId;
            NS_LOG_INFO("Selected suitable channel: " << e.channelId);

            // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
            Simulator::Schedule(Seconds(SCAN_STATUS_VALID),
                                &Dect2020Mac::OperatingChannelSelection,
                                this);
            // Start the beacon transmission
            StartBeaconTransmission();
            return;
        }
    }

    // 3. Search channel with the lowest busy / lowest possible subslots
    uint32_t lowestBusy = std::numeric_limits<uint32_t>::max();
    uint32_t lowestPossible = std::numeric_limits<uint32_t>::max();
    uint32_t selectedChannelId = 0;

    for (const auto& e : evaluations)
    {
        // Number of busy subslots smaller --> better
        if (e.busy < lowestBusy)
        {
            lowestBusy = e.busy;
            lowestPossible = e.possible;
            selectedChannelId = e.channelId;
        }
        // Number of busy subslots equal --> number of possible subslots smaller (= number of free
        // subslots bigger) --> better
        else if (e.busy == lowestBusy)
        {
            if (e.possible < lowestPossible)
            {
                lowestPossible = e.possible;
                selectedChannelId = e.channelId;
            }
        }
    }

    m_operatingChannelId = selectedChannelId;
    NS_LOG_INFO("Selected the channel with the lowest number of busy / possible subslots: "
                << m_operatingChannelId);

    // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
    Simulator::Schedule(Seconds(SCAN_STATUS_VALID), &Dect2020Mac::OperatingChannelSelection, this);
    // Start the beacon transmission
    StartBeaconTransmission();
}

Dect2020PhysicalHeaderField
Dect2020Mac::CreatePhysicalHeaderField()
{
    Dect2020PhysicalHeaderField physicalHeaderField;
    physicalHeaderField.SetPacketLengthType(0); // 0 = in Subslots, 1 = in Slots
    physicalHeaderField.SetPacketLength(5);     // Size of packet in slots/subslots

    // Short Network ID: The last 8 LSB bits of the Network ID # ETSI 103 636 04 4.2.3.1
    uint8_t shortNetworkID = m_networkId & 0xFF;
    physicalHeaderField.SetShortNetworkID(shortNetworkID);
    physicalHeaderField.SetTransmitterIdentity(m_shortRadioDeviceId);
    physicalHeaderField.SetTransmitPower(3);
    physicalHeaderField.SetDFMCS(m_mcs);

    return physicalHeaderField;
}

Dect2020PhysicalHeaderField
Dect2020Mac::CreatePhysicalHeaderField(uint8_t packetLengthType, uint32_t packetLengthBytes)
{
    Dect2020PhysicalHeaderField physicalHeaderField;
    physicalHeaderField.SetPacketLengthType(packetLengthType); // 0 = in Subslots, 1 = in Slots

    if (packetLengthType == 0) // Packet length is given in subslots
    {
        // TBD
    }
    else if (packetLengthType == 1) // Packet length is given in slots
    {
        // DEBUG:
        double packetLengthBits = packetLengthBytes * 8;
        auto transportBlockSizeBits =
            this->m_phy->GetMcsTransportBlockSize(m_subcarrierScalingFactor,
                                                  m_fourierTransformScalingFactor,
                                                  m_mcs);

        NS_LOG_INFO("packetLengthBits: " << packetLengthBits);
        NS_LOG_INFO("transportBlockSizeBits: " << transportBlockSizeBits);

        double mcsTransportBlockSizeBits =
            this->m_phy->GetMcsTransportBlockSize(m_subcarrierScalingFactor,
                                                  m_fourierTransformScalingFactor,
                                                  m_mcs);

        uint16_t packetLengthInSlots = 0;
        packetLengthInSlots = std::ceil(packetLengthBits / mcsTransportBlockSizeBits);

        physicalHeaderField.SetPacketLength(
            packetLengthInSlots); // Size of packet in slots/subslots
    }

    // Short Network ID: The last 8 LSB bits of the Network ID # ETSI 103 636 04 4.2.3.1
    uint8_t shortNetworkID = m_networkId & 0xFF;
    physicalHeaderField.SetShortNetworkID(shortNetworkID);
    physicalHeaderField.SetTransmitterIdentity(m_shortRadioDeviceId);
    physicalHeaderField.SetTransmitPower(3); // TODO: Wie wird das bestimmt?
    physicalHeaderField.SetDFMCS(m_mcs);     // TODO: Wie wird das bestimmt?

    return physicalHeaderField;
}

uint32_t
Dect2020Mac::GenerateValidNetworkId()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint32_t networkId = 0;
    uint8_t lsb;
    uint32_t msb;
    do
    {
        networkId = randomVar->GetValue(1, std::numeric_limits<uint32_t>::max());
        lsb = networkId & 0xFF;
        msb = (networkId >> 8) & 0xFFFFFF;
    } while (lsb == 0x00 || msb == 0x000000);

    // DEBUG: networkId auf festen Wert setzen. Später random erstellen und PT Device Beacon
    // empfangen lassen
    networkId = 123456;
    return networkId;
}

void
Dect2020Mac::SetCurrentChannelId(uint32_t channelId)
{
    NS_LOG_FUNCTION(this << channelId);
    m_currentChannelId = channelId;
    if (m_operatingChannelId == 0)
    {
        m_operatingChannelId = channelId;
    }
}

void
Dect2020Mac::SetNetworkId(uint32_t networkId)
{
    NS_LOG_FUNCTION(this << networkId);

    // Check for valid Network Ids
    uint8_t lsb = networkId & 0xFF;             // Short Network ID (last 8 Bit)
    uint32_t msb = (networkId >> 8) & 0xFFFFFF; // First 24 Bit of the Network ID (globally unique)

    if (lsb == 0x00 || msb == 0x000000)
    {
        NS_LOG_ERROR(Simulator::Now().GetMilliSeconds()
                     << ": Invalid Network ID: LSB and MSB have to be greater zero.");
        return;
    }

    m_networkId = networkId;

    NS_LOG_DEBUG(Simulator::Now().GetMilliSeconds()
                 << ": Network ID set: 0x" << std::hex << std::setw(8) << std::setfill('0')
                 << m_networkId << " on Device " << this);
}

uint32_t
Dect2020Mac::GetNetworkId() const
{
    return m_networkId;
}

uint32_t
Dect2020Mac::GenerateLongRadioDeviceId()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint32_t rdId = 0;

    do
    {
        rdId = randomVar->GetValue(1, 0xFFFFFFFD); // Range 0x00000001 bis 0xFFFFFFFD
    } while (rdId == 0x00000000 || rdId == 0xFFFFFFFE || rdId == 0xFFFFFFFF);

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Generated Long Radio Device ID: 0x" << std::hex << std::setw(8)
                << std::setfill('0') << rdId);
    return rdId;
}

void
Dect2020Mac::SetLongRadioDeviceId(uint32_t rdId)
{
    if (!(rdId == 0x00000000))
    {
        m_longRadioDeviceId = rdId;

        NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                    << ": Set Long Radio Device ID: 0x" << std::hex << std::setw(8)
                    << std::setfill('0') << rdId << " on Device " << this);
    }
    else
    {
        NS_LOG_WARN(Simulator::Now().GetMilliSeconds()
                    << ": Invalid Long Radio Device ID detected. Generate a new one.");

        SetLongRadioDeviceId(GenerateLongRadioDeviceId());
        // Hier Short RDID
    }
}

uint32_t
Dect2020Mac::GetLongRadioDeviceId() const
{
    return m_longRadioDeviceId;
}

uint16_t
Dect2020Mac::GenerateShortRadioDeviceId()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint16_t rdId = 0;

    do
    {
        rdId = randomVar->GetValue(1, 0xFFFE); // Range 0x0001 bis 0xFFFE
    } while (rdId == 0x0000 || rdId == 0xFFFF);

    NS_LOG_DEBUG(Simulator::Now().GetMilliSeconds()
                 << ": Short Radio Device ID " << rdId << "generated.");

    return rdId;
}

void
Dect2020Mac::SetShortRadioDeviceId(uint16_t rdId)
{
    if (!(rdId == 0x0000))
    {
        m_shortRadioDeviceId = rdId;
    }
    else
    {
        NS_LOG_WARN(Simulator::Now().GetMilliSeconds()
                    << ": Invalid Short Radio Device ID detected. Generate a new one.");

        SetShortRadioDeviceId(GenerateShortRadioDeviceId());
    }
}

uint16_t
Dect2020Mac::GetShortRadioDeviceId() const
{
    return m_shortRadioDeviceId;
}

void
Dect2020Mac::InitializeDevice()
{
    SetLongRadioDeviceId(GenerateLongRadioDeviceId());
    SetShortRadioDeviceId(GenerateShortRadioDeviceId());
}
} // namespace ns3
