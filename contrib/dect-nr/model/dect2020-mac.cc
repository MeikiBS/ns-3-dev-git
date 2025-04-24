/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-mac.h"

#include "dect2020-beacon-header.h"
#include "dect2020-beacon-message.h"
#include "dect2020-mac-header-type.h"
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
    m_currentChannelId = 1658;

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
    Dect2020BeaconMessage beaconMessage;
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
Dect2020Mac::StartBeaconTransmission()
{
    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Start Beacon Transmission on channel: " << m_currentChannelId);

    Ptr<Packet> networkBeacon = Create<Packet>();

    NS_LOG_INFO("Größe des Pakets direkt nach Erstellung: " << networkBeacon->GetSize());
    // MAC Header Type
    Dect2020MacHeaderType macHeaderType;
    macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::BEACON_HEADER);
    networkBeacon->AddHeader(macHeaderType);
    NS_LOG_INFO("Größe des Pakets direkt nach macHeaderType: " << networkBeacon->GetSize());

    // Mac Beacon Header
    Dect2020BeaconHeader beaconHeader;
    beaconHeader.SetNetworkId(m_networkId);
    beaconHeader.SetTransmitterAddress(m_longRadioDeviceId);

    networkBeacon->AddHeader(beaconHeader);

    NS_LOG_INFO("Größe des Pakets direkt nach beaconHeader: " << networkBeacon->GetSize());

    // MAC Beacon Message
    Dect2020BeaconMessage beaconMessage;
    // TODO: beaconMessage mit Inhalt füllen

    networkBeacon->AddHeader(beaconMessage);

    NS_LOG_INFO("Größe des Pakets direkt nach beaconMessage: " << networkBeacon->GetSize());

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": StartBeaconTransmission() aufgerufen von 0x" << std::hex
                << this->GetLongRadioDeviceId() << " übergibt Paket mit der Größe "
                << networkBeacon->GetSize() << " Bytes und UID " << networkBeacon->GetUid()
                << " an PHY.");

    m_phy->Send(networkBeacon, CreatePhysicalHeaderField());

    // NS_LOG_INFO("Network Beacon gesendet von Gerät 0x"
    //             << std::hex  << this->GetLongRadioDeviceId());
    // NS_LOG_INFO("MAC Header Type: " << macHeaderType.GetMacHeaderTypeField());

    Simulator::Schedule(MilliSeconds(beaconMessage.GetNetworkBeaconPeriodTime()),
                        &Dect2020Mac::StartBeaconTransmission,
                        this);
}

void
Dect2020Mac::OperatingChannelSelection()
{
    std::vector<ChannelEvaluation> evaluations;

    for (auto& channel : m_phy->m_channels)
    {
        ChannelEvaluation eval;
        eval.channelId = channel.m_channelId;

        if (channel.m_channelId == 1657)
        {
            Dect2020SpectrumModelManager::AddSpectrumPowerToChannel(1657, -100);
        }

        for (int i = 0; i < SCAN_MEAS_DURATION; i++)
        {
            Slot* currentSlot = m_phy->GetCurrentSlot(channel.m_channelId);

            for (auto& subslot : currentSlot->subslots)
            {
                NS_LOG_INFO("DEBUG: Subslot " << subslot.subslotId << " in Slot "
                                              << currentSlot->slotId);
                double rssi = Dect2020SpectrumModelManager::GetRssiDbm(channel.m_channelId);

                if (rssi <= RSSI_THRESHOLD_MIN)
                {
                    eval.free++;
                }
                else if (rssi <= RSSI_THRESHOLD_MAX)
                {
                    eval.possible++;
                }
                else
                {
                    eval.busy++;
                }
            }
        }

        evaluations.push_back(eval);
    }

    // 1. Search 100 % free channel
    for (const auto& e : evaluations)
    {
        uint32_t total = e.Total();

        if (e.free == total)
        {
            m_currentChannelId = e.channelId;
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
            m_currentChannelId = e.channelId;
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

    m_currentChannelId = selectedChannelId;
    NS_LOG_INFO("Selected the channel with the lowest number of busy / possible subslots: " << m_currentChannelId);

    // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
    Simulator::Schedule(Seconds(SCAN_STATUS_VALID), &Dect2020Mac::OperatingChannelSelection, this);
    // Start the beacon transmission
    StartBeaconTransmission();
}

Dect2020PhysicalHeaderField
Dect2020Mac::CreatePhysicalHeaderField()
{
    Dect2020PhysicalHeaderField physicalHeaderField;
    physicalHeaderField.SetPacketLengthType(1); // TODO: Wie wird das bestimmt?
    physicalHeaderField.SetPacketLength(5);     // TODO: Wie wird das bestimmt?

    // Short Network ID: The last 8 LSB bits of the Network ID # ETSI 103 636 04 4.2.3.1
    uint8_t shortNetworkID = m_networkId & 0xFF;
    physicalHeaderField.SetShortNetworkID(shortNetworkID);
    physicalHeaderField.SetTransmitterIdentity(m_shortRadioDeviceId);
    physicalHeaderField.SetTransmitPower(3); // TODO: Wie wird das bestimmt?
    physicalHeaderField.SetDFMCS(0);         // TODO: Wie wird das bestimmt?

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
