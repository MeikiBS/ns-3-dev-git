/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-mac.h"

#include "dect2020-net-device.h"
#include "dect2020-phy.h"
#include "dect2020-beacon-header.h"
#include "dect2020-beacon-message.h"

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
                            .AddConstructor<Dect2020Mac>()
        // Hier können weitere Attribute und Trace-Quellen hinzugefügt werden
        ;
    return tid;
}

Dect2020Mac::Dect2020Mac()
{
    NS_LOG_FUNCTION(this);
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
}

void
Dect2020Mac::Send(Ptr<Packet> packet, const Address& dest, Dect2020PacketType type)
{
    NS_LOG_FUNCTION(this << packet << dest << type);

    // Hier können MAC-Header hinzugefügt werden
    if(type == BEACON)
    {
        Dect2020BeaconHeader beaconHeader;
        beaconHeader.SetNetworkId(this->GetNetworkId());
        beaconHeader.SetTransmitterAddress(this->GetLongRadioDeviceId());
    }

    // Senden des Pakets über die PHY-Schicht
    m_phy->Send(packet);

    // Trace-Aufruf
    m_txPacketTrace(packet);
}

void
Dect2020Mac::ReceiveFromPhy(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

    // Hier können MAC-Header entfernt und überprüft werden

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
        m_networkId = 123456;
        JoinNetwork(m_networkId);
    }
}

void
Dect2020Mac::InitializeNetwork()
{
    uint32_t networkId = GenerateValidNetworkId();
    SetNetworkId(networkId);

    NS_LOG_INFO("FT-Device " << this << " started a new Network with the Network ID: " << std::hex
                             << networkId);

    StartBeaconTransmission();
}

void
Dect2020Mac::JoinNetwork(uint32_t networkId)
{
    SetNetworkId(networkId);

    NS_LOG_INFO("PT-Device joined a Network with the Network ID: " << std::hex << networkId);

    SetShortRadioDeviceId(GenerateShortRadioDeviceId());
    // TODO: Unter welchen Umständen kann einem Network beigetreten/nicht beigetreten werden?
}

void
Dect2020Mac::StartBeaconTransmission()
{
    // tbd
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
        NS_LOG_ERROR("Invalid Network ID: LSB and MSB have to be greater zero.");
        return;
    }

    m_networkId = networkId;

    NS_LOG_DEBUG("Network ID set: 0x" << std::hex << std::setw(8) << std::setfill('0')
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

    NS_LOG_INFO("Generated Long Radio Device ID: 0x" << std::hex << std::setw(8)
                                                     << std::setfill('0') << rdId);
    return rdId;
}

void
Dect2020Mac::SetLongRadioDeviceId(uint32_t rdId)
{
    if (!(rdId == 0x00000000))
    {
        m_longRadioDeviceId = rdId;

        NS_LOG_INFO("Set Long Radio Device ID: 0x" << std::hex << std::setw(8) << std::setfill('0')
                                                   << rdId << " on Device " << this);
    }
    else
    {
        NS_LOG_WARN("Invalid Long Radio Device ID detected. Generate a new one.");

        SetLongRadioDeviceId(GenerateLongRadioDeviceId());
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

    NS_LOG_DEBUG("Short Radio Device ID " << rdId << "generated.");

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
        NS_LOG_WARN("Invalid Short Radio Device ID detected. Generate a new one.");

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
}
} // namespace ns3
