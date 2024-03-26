#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dect-channel.h"
#include "ns3/dect-nr-helper.h"
#include "ns3/end-device-dect-nr-mac.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/network-module.h"
#include "ns3/node-container.h"
#include "ns3/point-to-point-module.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device-helper.h"

/**
 * \file
 *
 * Explain here what the example does.
 */

using namespace ns3;
using namespace dect2020;

int
main(int argc, char* argv[])
{
    NS_LOG_COMPONENT_DEFINE("SimpleExample"); // Define a component for logging

    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);

    cmd.Parse(argc, argv);

    /* ... */
    NS_LOG_INFO("Hello World");

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Erstelle einen NodeContainer, der zwei Nodes enthält
    NodeContainer nodes;
    nodes.Create(2);

    // Erstelle ein SimpleNetDeviceHelper Objekt
    SimpleNetDeviceHelper netDeviceHelper;
    netDeviceHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    netDeviceHelper.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices;
    devices = netDeviceHelper.Install(nodes);

    // Ausgabe der Typen der Devices
    std::cout << "devices.Get(0)->GetTypeId().GetName(): " << devices.Get(0)->GetTypeId().GetName()
              << std::endl;

    // Erstelle ein InternetStackHelper Objekt --> Installiert IPv4 auf Nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Erstelle ein Ipv4AddressHelper Objekt für IP Adressvergabe
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    // Der Ipv4InterfaceContainer ordnet den Devices IP Adresse zu. Im Container selbst stehen
    // die Ipv4 Stacks (IP, UDP/TCP) und die IDs der Devices.
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
