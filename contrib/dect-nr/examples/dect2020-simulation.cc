// #include "ns3/core-module.h"
// #include "ns3/network-module.h"
// #include "ns3/dect2020-module.h"

// using namespace ns3;

// int main(int argc, char *argv[])
// {
//   // Erstellen der Knoten
//   Ptr<Node> node1 = CreateObject<Node>();
//   Ptr<Node> node2 = CreateObject<Node>();

//   // Erstellen der NetDevices
//   Ptr<Dect2020NetDevice> dev1 = CreateObject<Dect2020NetDevice>();
//   Ptr<Dect2020NetDevice> dev2 = CreateObject<Dect2020NetDevice>();

//   // Erstellen der MAC-Schichten
//   Ptr<Dect2020Mac> mac1 = CreateObject<Dect2020Mac>();
//   Ptr<Dect2020Mac> mac2 = CreateObject<Dect2020Mac>();

//   // Erstellen der PHY-Schichten
//   Ptr<Dect2020Phy> phy1 = CreateObject<Dect2020Phy>();
//   Ptr<Dect2020Phy> phy2 = CreateObject<Dect2020Phy>();

//   // Erstellen des Kanals
//   Ptr<Dect2020Channel> channel = CreateObject<Dect2020Channel>();

//   // Verbinden der Komponenten für Gerät 1
//   dev1->SetMac(mac1);
//   dev1->SetPhy(phy1);
//   mac1->SetNetDevice(dev1);
//   mac1->SetPhy(phy1);
//   phy1->SetMac(mac1);
//   phy1->SetNetDevice(dev1);
//   phy1->SetChannel(channel);

//   // Verbinden der Komponenten für Gerät 2
//   dev2->SetMac(mac2);
//   dev2->SetPhy(phy2);
//   mac2->SetNetDevice(dev2);
//   mac2->SetPhy(phy2);
//   phy2->SetMac(mac2);
//   phy2->SetNetDevice(dev2);
//   phy2->SetChannel(channel);

//   // Hinzufügen der PHYs zum Kanal
//   channel->AddPhy(phy1);
//   channel->AddPhy(phy2);

//   // Setzen der Adressen
//   dev1->SetAddress(Mac48Address::Allocate());
//   dev2->SetAddress(Mac48Address::Allocate());

//   // Hinzufügen der Geräte zu den Knoten
//   node1->AddDevice(dev1);
//   node2->AddDevice(dev2);

//   // Manuelles Setzen des Link-Status
//   dev1->SetLinkUp();
//   dev2->SetLinkUp();

//   // Empfangs-Callback für Gerät 2
//   dev2->SetReceiveCallback(MakeCallback([](Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, const Address& sender) {
//     NS_LOG_UNCOND("Gerät 2 hat ein Paket empfangen von " << Mac48Address::ConvertFrom(sender));
//   }));

//   dev2->SetReceiveCallback(MakeCallback(&ReceivePacket));

//   // Senden eines Pakets von Gerät 1 zu Gerät 2 nach 1 Sekunde
//   Simulator::Schedule(Seconds(1.0), &Dect2020NetDevice::Send, dev1, Create<Packet>(100), dev2->GetAddress(), 0);

//   Simulator::Run();
//   Simulator::Destroy();
//   return 0;
// }


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/dect2020-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Dect2020Simulation");

// Empfangsfunktion definieren
bool ReceivePacket (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, const Address &sender)
{
  NS_LOG_UNCOND ("Gerät 2 hat ein Paket empfangen von " << Mac48Address::ConvertFrom (sender));
  return true; // Paket wurde erfolgreich empfangen
}

int main (int argc, char *argv[])
{
  // Protokollierung aktivieren
  LogComponentEnable ("Dect2020NetDevice", LOG_LEVEL_INFO);
  LogComponentEnable ("Dect2020Mac", LOG_LEVEL_INFO);
  LogComponentEnable ("Dect2020Phy", LOG_LEVEL_INFO);
  LogComponentEnable ("Dect2020Channel", LOG_LEVEL_INFO);
  LogComponentEnable ("Dect2020Simulation", LOG_LEVEL_INFO);

  // Erstellen der Knoten
  NodeContainer nodes;
  nodes.Create (2);

  // Erstellen des Kanals
  Ptr<Dect2020Channel> channel = CreateObject<Dect2020Channel> ();

  // Erstellen und Konfigurieren der Geräte
  NetDeviceContainer devices;
  for (uint32_t i = 0; i < nodes.GetN (); ++i)
    {
      // Erstellen des NetDevice
      Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice> ();

      // Erstellen der MAC- und PHY-Schichten
      Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac> ();
      Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy> ();

      // Verbinden der Komponenten
      dev->SetMac (mac);
      dev->SetPhy (phy);
      mac->SetNetDevice (dev);
      mac->SetPhy (phy);
      phy->SetMac (mac);
      phy->SetNetDevice (dev);
      phy->SetChannel (channel);
      channel->AddPhy (phy);

      // Hinzufügen des Geräts zum Knoten
      nodes.Get (i)->AddDevice (dev);

      // Setzen der MAC-Adresse
      dev->SetAddress (Mac48Address::Allocate ());

      // Manuelles Setzen des Link-Status
      dev->SetLinkUp ();

      devices.Add (dev);
    }

  // Setzen des Empfangs-Callbacks für Gerät 2
  devices.Get (1)->SetReceiveCallback (MakeCallback (&ReceivePacket));

  // Installieren des Internet-Stacks
  InternetStackHelper internet;
  internet.Install (nodes);

  // Zuweisen von IP-Adressen
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (devices);

  // Erstellen der Anwendung auf Knoten 1 (Empfänger)
  uint16_t port = 9;
  UdpEchoServerHelper server (port);
  ApplicationContainer serverApp = server.Install (nodes.Get (1));
  serverApp.Start (Seconds (1.0));
  serverApp.Stop (Seconds (10.0));

  // Erstellen der Anwendung auf Knoten 0 (Sender)
  UdpEchoClientHelper client (interfaces.GetAddress (1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (1));
  client.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApp = client.Install (nodes.Get (0));
  clientApp.Start (Seconds (2.0));
  clientApp.Stop (Seconds (10.0));

  // Simulation ausführen
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
