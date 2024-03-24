#include "ns3/core-module.h"
#include "ns3/dect-channel.h"
#include "ns3/dect-nr-helper.h"
#include "ns3/end-device-dect-nr-mac.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
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
    NS_LOG_COMPONENT_DEFINE("Dect2020Example"); // Define a component for logging

    bool verbose = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);

    cmd.Parse(argc, argv);

    /* ... */
    NS_LOG_INFO("Hello World");

    // Erstelle einen DECT Kanal
    Ptr<Dect2020Channel> channel = CreateObject<Dect2020Channel>();

    // ############################
    // ###### DECT TX DEVICE ######
    // ############################

    // Erstelle ein DECT-PHY-TX-Objekt
    Ptr<DectPhy> phyTx = CreateObject<DectPhy>();

    // Erstelle ein DECT-MAC-TX-Objekt
    Ptr<EndDeviceDect2020Mac> macTx = CreateObject<EndDeviceDect2020Mac>();

    // Weise dem PHY-TX-Objekt einen Channel zu
    phyTx->SetChannel(channel);

    // Erstelle ein DECT-TX-Netzwerkgerät
    Ptr<DectNetDevice> dectDeviceTx = CreateObject<DectNetDevice>();
    dectDeviceTx->SetMac(macTx);
    dectDeviceTx->SetPhy(phyTx);
    dectDeviceTx->SetChannel(channel);

    // ############################
    // ###### DECT RX DEVICE ######
    // ############################

    // Erstelle ein DECT-PHY-RX-Objekt
    Ptr<DectPhy> phyRx = CreateObject<DectPhy>();

    // Erstelle ein DECT-MAC-RX-Objekt
    Ptr<EndDeviceDect2020Mac> macRx = CreateObject<EndDeviceDect2020Mac>();

    // Weise dem PHY-RX-Objekt einen Channel zu
    phyRx->SetChannel(channel);

    // Erstelle ein DECT-RX-Netzwerkgerät
    Ptr<DectNetDevice> dectDeviceRx = CreateObject<DectNetDevice>();
    dectDeviceRx->SetMac(macRx);
    dectDeviceRx->SetPhy(phyRx);
    dectDeviceRx->SetChannel(channel);

    // Erstelle einen NodeContainer
    NodeContainer nodes;
    nodes.Create(2);
    nodes.Get(0)->AddDevice(dectDeviceTx);
    nodes.Get(1)->AddDevice(dectDeviceRx);

    SimpleNetDeviceHelper helper = SimpleNetDeviceHelper();
    helper.Install(nodes, channel);

    Ptr<Packet> packet = Create<Packet>(1024);
    // dectDeviceTx->Send(packet, macRx->m_macAddress, 0);

    // Ptr<Mac48Address> newMac = CreateObject<Mac48Address>("MEIKEL");
    if (packet != nullptr)
    {
        NS_LOG_INFO("Packet is not null");
    }
    else
    {
        NS_LOG_ERROR("Packet is null");
    }

    // auto macAddr = dectDeviceRx->GetMac()->m_macAddress;
    Mac48Address macAddr("01:02:03:04:05:06");

    if (dectDeviceTx != nullptr) // macRx->m_macAddress != nullptr
    {
        std::cout << "dectDeviceTx: " << dectDeviceTx << std::endl;
        dectDeviceTx->Test();
        dectDeviceTx->Send(packet, macAddr, 0x0800);
    }
    else
    {
        NS_LOG_ERROR("macRx->m_macAddress is null");
    }

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
