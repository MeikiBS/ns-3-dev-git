#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dect2020-beacon-header.h"
#include "ns3/dect2020-beacon-message.h"
#include "ns3/dect2020-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"

using namespace ns3;
using Dect2020NetDevice::TerminationPointType::FT;
using Dect2020NetDevice::TerminationPointType::PT;

NS_LOG_COMPONENT_DEFINE("Dect2020Simulation");

void
TestBeaconMessage()
{
    Dect2020BeaconMessage originalMessage;

    originalMessage.SetTxPowerIncluded(0);
    originalMessage.SetPowerConstraints(1);
    originalMessage.SetCurrentClusterChannelIncluded(0);
    originalMessage.SetNetworkBeaconChannels(3);
    originalMessage.SetNetworkBeaconPeriod(15);
    originalMessage.SetClusterBeaconPeriod(15);
    originalMessage.SetNextClusterChannel(8191);
    originalMessage.SetTimeToNext(9999);

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(originalMessage);

    Dect2020BeaconMessage receivedMessage;
    packet->RemoveHeader(receivedMessage);

    originalMessage.Print(std::cout);
    std::cout << std::endl << std::endl;
    receivedMessage.Print(std::cout);
}

void
TestBeaconHeader()
{
    // Originalwerte
    uint32_t originalNetworkId = 0x123456;       // Beispielwert
    uint32_t originalTransmitterId = 0xABCDEF01; // Beispielwert

    // Erstellen und Setzen des Headers
    Dect2020BeaconHeader header;
    header.SetNetworkId(originalNetworkId);
    header.SetTransmitterAddress(originalTransmitterId);

    // Serialisieren
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(header);

    // Deserialisieren
    Dect2020BeaconHeader deserializedHeader;
    packet->RemoveHeader(deserializedHeader);

    // Überprüfen der Werte
    uint32_t deserializedNetworkId = deserializedHeader.GetNetworkId();
    uint32_t deserializedTransmitterId = deserializedHeader.GetTransmitterAddress();

    NS_LOG_INFO("originalNetworkId: " << originalNetworkId);
    NS_LOG_INFO("deserializedNetworkId: " << deserializedNetworkId);

    if (originalNetworkId == deserializedNetworkId)
    {
        NS_LOG_INFO("true");
    }
    else
    {
        NS_LOG_INFO("false");
    }

    if (originalTransmitterId == deserializedTransmitterId)
    {
        NS_LOG_INFO("true");
    }
    else
    {
        NS_LOG_INFO("false");
    }
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
    // Protokollierung aktivieren
    LogComponentEnable("Dect2020NetDevice", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Mac", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Phy", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Channel", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Simulation", LOG_LEVEL_INFO);

    // Erstellen der Knoten
    NodeContainer nodes;
    nodes.Create(2);

    // Erstellen des Kanals
    Ptr<Dect2020Channel> channel = CreateObject<Dect2020Channel>();

    // Erstellen und Konfigurieren der Geräte
    NetDeviceContainer devices;
    for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
        // Erstellen des NetDevice
        Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();

        if (i == 0)
        {
            dev->SetTerminationPointType(FT); // Erstes Gerät ist FT
        }
        else
        {
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
        phy->SetNetDevice(dev);
        phy->SetChannel(channel);
        channel->AddPhy(phy);

        mac->Start();

        // Hinzufügen des Geräts zum Knoten
        nodes.Get(i)->AddDevice(dev);

        // Setzen der MAC-Adresse
        dev->SetAddress(Mac48Address::Allocate());

        // Manuelles Setzen des Link-Status
        dev->SetLinkUp();

        devices.Add(dev);
    }

    // Setzen des Empfangs-Callbacks für Gerät 2
    devices.Get(1)->SetReceiveCallback(MakeCallback(&ReceivePacket));

    // Installieren des Internet-Stacks
    InternetStackHelper internet;
    internet.Install(nodes);

    // Zuweisen von IP-Adressen
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    // Erstellen der Anwendung auf Knoten 1 (Empfänger)
    uint16_t port = 9;
    UdpEchoServerHelper server(port);
    ApplicationContainer serverApp = server.Install(nodes.Get(1));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(10.0));

    // Erstellen der Anwendung auf Knoten 0 (Sender)
    UdpEchoClientHelper client(interfaces.GetAddress(1), port);
    client.SetAttribute("MaxPackets", UintegerValue(5));
    client.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApp = client.Install(nodes.Get(0));
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(Seconds(10.0));

    // Simulation ausführen
    Simulator::Run();
    Simulator::Destroy();

    // TestBeaconHeader();
    TestBeaconMessage();

    return 0;
}