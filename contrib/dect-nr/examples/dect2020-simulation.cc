#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dect2020-beacon-header.h"
#include "ns3/dect2020-beacon-message.h"
#include "ns3/dect2020-mac-header-type.h"
#include "ns3/dect2020-mac.h"
#include "ns3/dect2020-net-device.h"
#include "ns3/dect2020-phy.h"
#include "ns3/dect2020-physical-header-field.h"
#include "ns3/dect2020-spectrum-signal-parameters.h"
#include "ns3/internet-module.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/network-module.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/spectrum-analyzer-helper.h"
#include "ns3/spectrum-analyzer.h"

// using namespace ns3;
// using Dect2020NetDevice::TerminationPointType::FT;
// using Dect2020NetDevice::TerminationPointType::PT;

using namespace ns3;
using TermPointType = Dect2020NetDevice::TerminationPointType;

NS_LOG_COMPONENT_DEFINE("Dect2020Simulation");

void
TestMacHeaderField()
{
    Dect2020MacHeaderType originalMessage;
    originalMessage.SetVersion(0);
    originalMessage.SetMacSecurity(Dect2020MacHeaderType::MAC_SECURITY_NOT_USED);
    originalMessage.SetMacHeaderTypeField(Dect2020MacHeaderType::BEACON_HEADER);

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(originalMessage);

    Dect2020MacHeaderType receivedMessage;
    packet->RemoveHeader(receivedMessage);

    originalMessage.Print(std::cout);
    std::cout << std::endl << std::endl;
    receivedMessage.Print(std::cout);
}

void
TestBeaconMessage()
{
    Dect2020NetworkBeaconMessage originalMessage;

    originalMessage.SetTxPowerIncluded(0);
    originalMessage.SetPowerConstraints(1);
    originalMessage.SetCurrentClusterChannelIncluded(0);
    originalMessage.SetNetworkBeaconChannels(1);
    originalMessage.SetNetworkBeaconPeriod(NETWORK_PERIOD_2000MS);
    originalMessage.SetClusterBeaconPeriod(CLUSTER_PERIOD_16000MS);
    originalMessage.SetNextClusterChannel(8191);
    originalMessage.SetTimeToNext(UINT32_MAX - 1);

    uint16_t tempArr[3] = {123, 456, 789};
    originalMessage.SetAdditionalNetworkBeaconChannels(tempArr);

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(originalMessage);

    Dect2020NetworkBeaconMessage receivedMessage;
    packet->RemoveHeader(receivedMessage);

    originalMessage.Print(std::cout);
    std::cout << std::endl << std::endl;
    receivedMessage.Print(std::cout);
}

// void
// TestBeaconHeader()
// {
//     // Originalwerte
//     uint32_t originalNetworkId = 0x123456;       // Beispielwert
//     uint32_t originalTransmitterId = 0xABCDEF01; // Beispielwert

//     // Erstellen und Setzen des Headers
//     Dect2020BeaconHeader header;
//     header.SetNetworkId(originalNetworkId);
//     header.SetTransmitterAddress(originalTransmitterId);

//     // Serialisieren
//     Ptr<Packet> packet = Create<Packet>();
//     packet->AddHeader(header);

//     // Deserialisieren
//     Dect2020BeaconHeader deserializedHeader;
//     packet->RemoveHeader(deserializedHeader);

//     // Überprüfen der Werte
//     uint32_t deserializedNetworkId = deserializedHeader.GetNetworkId();
//     uint32_t deserializedTransmitterId = deserializedHeader.GetTransmitterAddress();

//     NS_LOG_INFO("originalNetworkId: " << originalNetworkId);
//     NS_LOG_INFO("deserializedNetworkId: " << deserializedNetworkId);

//     if (originalNetworkId == deserializedNetworkId)
//     {
//         NS_LOG_INFO("true");
//     }
//     else
//     {
//         NS_LOG_INFO("false");
//     }

//     if (originalTransmitterId == deserializedTransmitterId)
//     {
//         NS_LOG_INFO("true");
//     }
//     else
//     {
//         NS_LOG_INFO("false");
//     }
// }

void
TestPhysicalLayerControlFieldType1()
{
    Dect2020PhysicalHeaderField originalPhysicalHeaderField;

    uint8_t originalPacketlLengthType = 1;
    uint8_t originalPacketLength = 13;
    uint8_t originalShortNetworkID = 55;
    uint16_t originalTransmitterIdentity = 13130;
    uint8_t originalTransmitPower = 3;
    uint8_t originalDFMCS = 2;

    originalPhysicalHeaderField.SetPacketLengthType(originalPacketlLengthType);
    originalPhysicalHeaderField.SetPacketLength(originalPacketLength);
    originalPhysicalHeaderField.SetShortNetworkID(originalShortNetworkID);
    originalPhysicalHeaderField.SetTransmitterIdentity(originalTransmitterIdentity);
    originalPhysicalHeaderField.SetTransmitPower(originalTransmitPower);
    originalPhysicalHeaderField.SetDFMCS(originalDFMCS);

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(originalPhysicalHeaderField);

    Dect2020PhysicalHeaderField deserializedPhysicalHeaderField;
    packet->RemoveHeader(deserializedPhysicalHeaderField);

    NS_LOG_INFO(originalPhysicalHeaderField);
    NS_LOG_INFO(deserializedPhysicalHeaderField);
}

// Empfangsfunktion definieren
bool
ReceivePacket(Ptr<NetDevice> device,
              Ptr<const Packet> packet,
              uint16_t protocol,
              const Address& sender)
{
    NS_LOG_UNCOND("Gerät 2 hat ein Paket empfangen von " << Mac48Address::ConvertFrom(sender));
    return true; // Paket wurde erfolgreich empfangen
}

int
main(int argc, char* argv[])
{
    Simulator::Stop(Seconds(5));

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds());

    // Protokollierung aktivieren
    LogComponentEnable("Dect2020NetDevice", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Mac", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Phy", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Channel", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Simulation", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020SpectrumSignalParameters", LOG_LEVEL_INFO);

    // Hier Bereich für Tests
    // ###########################
    Dect2020SpectrumSignalParameters params;

    // ###########################

    // Erstellen der Knoten
    NodeContainer nodes;
    nodes.Create(3);

    // Erstellen des Kanals
    Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();

    // Erstellen und Konfigurieren der Geräte
    NetDeviceContainer devices;
    for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
        // Erstellen des NetDevice
        Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();

        if (i == 0)
        {
            TermPointType FT = TermPointType::FT;
            dev->SetTerminationPointType(FT); // Erstes Gerät ist FT
        }
        else
        {
            TermPointType PT = TermPointType::PT;
            dev->SetTerminationPointType(PT);
        }

        // Erstellen der MAC- und PHY-Schichten
        Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac>();
        Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy>();

        // Verbinden der Komponenten
        dev->SetMac(mac);
        dev->SetPhy(phy);
        mac->SetNetDevice(dev);
        mac->SetPhy(phy);
        phy->SetMac(mac);
        phy->SetDevice(dev);
        phy->SetChannel(channel);

        // mac->Start();

        // Hinzufügen des Geräts zum Knoten
        nodes.Get(i)->AddDevice(dev);

        // Setzen der MAC-Adresse
        dev->SetAddress(Mac48Address::Allocate());

        // Manuelles Setzen des Link-Status
        dev->SetLinkUp();

        devices.Add(dev);
        mac->Start();


        // for(int j = 0; j < 5; j++)
        // {
        //     if(dev->GetTerminationPointType() == TermPointType::PT)
        //     {
        //         Ptr<Packet> packet = Create<Packet>(100);
        //         Dect2020PhysicalHeaderField physicalHeaderField;

        //         Simulator::Schedule(MilliSeconds(1000), &Dect2020Phy::Send, phy, packet,
        //                              physicalHeaderField);
        //     }
        // }
    }


    // Ptr<Dect2020NetDevice> ft = DynamicCast<Dect2020NetDevice>(devices.Get(0));
    // ft->GetMac()->Start();

    // Setzen des Empfangs-Callbacks für Gerät 2
    // devices.Get(1)->SetReceiveCallback(MakeCallback(&ReceivePacket));

    // Simulation ausführen
    Simulator::Run();
    Simulator::Destroy();

    // TestBeaconHeader();
    // TestBeaconMessage();
    // TestMacHeaderField();

    // TestPhysicalLayerControlFieldType1();
    return 0;
}
