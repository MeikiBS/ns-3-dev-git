#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dect2020-channel-manager.h"
#include "ns3/dect2020-mac-header-type.h"
#include "ns3/dect2020-mac-information-elements.h"
#include "ns3/dect2020-mac-messages.h"
#include "ns3/dect2020-mac.h"
#include "ns3/dect2020-net-device.h"
#include "ns3/dect2020-phy.h"
#include "ns3/dect2020-physical-header-field.h"
#include "ns3/dect2020-spectrum-signal-parameters.h"
#include "ns3/internet-module.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/spectrum-analyzer-helper.h"
#include "ns3/spectrum-analyzer.h"
#include "ns3/spectrum-module.h"

#include <fstream>

using namespace ns3;
using TermPointType = Dect2020NetDevice::TerminationPointType;

int
main(int argc, char* argv[])
{
    Simulator::Stop(Seconds(30));

    LogComponentEnable("Dect2020NetDevice", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Mac", LOG_LEVEL_INFO);
    LogComponentEnable("Dect2020Phy", LOG_LEVEL_INFO);

    Dect2020ChannelManager channelManager;
    channelManager.InitializeChannels(1, 1); // Band 1, Scaling Factor 1

    // 2 FTs + 10 PTs
    NodeContainer ftNodes;
    ftNodes.Create(3);
    NodeContainer ptNodes;
    ptNodes.Create(50);

    MobilityHelper ftMobility;
    Ptr<ListPositionAllocator> ftPos = CreateObject<ListPositionAllocator>();
    ftPos->Add(Vector(0.0, 150.0, 0.0));
    ftPos->Add(Vector(75.0, 75.0, 0.0));
    ftPos->Add(Vector(150.0, 0.0, 0.0));
    ftMobility.SetPositionAllocator(ftPos);
    ftMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ftMobility.Install(ftNodes);

    MobilityHelper ptMobility;
    ptMobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                    "X",
                                    StringValue("ns3::UniformRandomVariable[Min=0.0|Max=150.0]"),
                                    "Y",
                                    StringValue("ns3::UniformRandomVariable[Min=0.0|Max=150.0]"));
    ptMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ptMobility.Install(ptNodes);
    // ptMobility.SetPositionAllocator("ns3::GridPositionAllocator",
    //                                 "MinX",
    //                                 DoubleValue(10.0),
    //                                 "MinY",
    //                                 DoubleValue(10.0),
    //                                 "DeltaX",
    //                                 DoubleValue(10.0),
    //                                 "DeltaY",
    //                                 DoubleValue(10.0),
    //                                 "GridWidth",
    //                                 UintegerValue(5),
    //                                 "LayoutType",
    //                                 StringValue("RowFirst"));
    // ptMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    // ptMobility.Install(ptNodes);

    // Kanalmodell
    Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
    Ptr<ConstantSpeedPropagationDelayModel> delayModel =
        CreateObject<ConstantSpeedPropagationDelayModel>();
    Ptr<LogDistancePropagationLossModel> lossModel =
        CreateObject<LogDistancePropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    channel->AddPropagationLossModel(lossModel);

    NetDeviceContainer devices;

    // FT-Devices
    for (uint32_t i = 0; i < ftNodes.GetN(); ++i)
    {
        Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();
        dev->SetBandNumber(1);
        dev->SetTerminationPointType(TermPointType::FT);

        Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac>();
        Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy>();

        dev->SetMac(mac);
        dev->SetPhy(phy);
        mac->SetNetDevice(dev);
        mac->SetPhy(phy);
        phy->SetMac(mac);
        ftNodes.Get(i)->AddDevice(dev);
        phy->SetDevice(dev);
        phy->SetMobility(ftNodes.Get(i)->GetObject<MobilityModel>());
        phy->SetChannel(channel);
        channel->AddRx(phy);

        dev->SetAddress(Mac48Address::Allocate());
        dev->SetLinkUp();
        devices.Add(dev);
        phy->Start();
        mac->Start();
    }

    // PT-Devices
    for (uint32_t i = 0; i < ptNodes.GetN(); ++i)
    {
        Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();
        dev->SetBandNumber(1);
        dev->SetTerminationPointType(TermPointType::PT);

        Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac>();
        Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy>();

        dev->SetMac(mac);
        dev->SetPhy(phy);
        mac->SetNetDevice(dev);
        mac->SetPhy(phy);
        phy->SetMac(mac);
        ptNodes.Get(i)->AddDevice(dev);
        phy->SetDevice(dev);
        phy->SetMobility(ptNodes.Get(i)->GetObject<MobilityModel>());
        phy->SetChannel(channel);
        channel->AddRx(phy);

        dev->SetAddress(Mac48Address::Allocate());
        dev->SetLinkUp();
        devices.Add(dev);
        phy->Start();
        mac->Start();
    }

    Simulator::Run();

    std::map<std::string, uint32_t> statusCounter;
    std::map<uint64_t, std::string> longRdidToName;

    std::ofstream topo("networkx_topology.py");
    topo << "import networkx as nx\n";
    topo << "import matplotlib.pyplot as plt\n";
    topo << "nodes = []\n";
    topo << "edges = []\n";

    int ftCounter = 1;
    int ptCounter = 1;

    for (uint32_t i = 0; i < devices.GetN(); ++i)
    {
        Ptr<Dect2020NetDevice> dev = DynamicCast<Dect2020NetDevice>(devices.Get(i));
        Ptr<Dect2020Mac> mac = dev->GetMac();
        Ptr<MobilityModel> mob = dev->GetNode()->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();

        auto status = mac->GetAssociationStatus();
        std::string statusStr;
        switch (status)
        {
        case 0:
            statusStr = "NOT_ASSOCIATED";
            break;
        case 1:
            statusStr = "ASSOCIATION_PREPARING";
            break;
        case 2:
            statusStr = "WAITING_FOR_SELECTED_FT";
            break;
        case 3:
            statusStr = "ASSOCIATION_PENDING";
            break;
        case 4:
            statusStr = "ASSOCIATED";
            break;
        default:
            statusStr = "UNKNOWN";
            break;
        }
        statusCounter[statusStr]++;

        std::string type = dev->GetTerminationPointType() == TermPointType::FT ? "FT" : "PT";
        std::string name;
        if (type == "FT")
        {
            name = "FT" + std::to_string(ftCounter++);
        }
        else
        {
            name = "PT" + std::to_string(ptCounter++);
        }

        longRdidToName[mac->GetLongRadioDeviceId()] = name;

        topo << "nodes.append(('" << name << "', '" << type << "', '" << statusStr << "', " << pos.x << ", " << pos.y << "))\n";


        if (status == 4) // ASSOCIATED
        {
            uint64_t ftId = mac->m_associatedFTNetDeviceLongRdId;
            std::string ftName = longRdidToName[ftId];
            topo << "edges.append(('" << ftName << "', '" << name << "'))\n";
        }

        NS_LOG_UNCOND("Device " << i << " LongRDID: 0x" << std::hex << mac->GetLongRadioDeviceId()
                                << " --> Association Status: " << statusStr);
    }

    NS_LOG_UNCOND("\nSummary of association statuses:");
    for (const auto& entry : statusCounter)
    {
        NS_LOG_UNCOND(entry.first << ": " << entry.second);
    }

    topo << R"(
G = nx.Graph()
for name, typ, status, x, y in nodes:
    G.add_node(name, type=typ, status=status, pos=(x, y))
G.add_edges_from(edges)
pos = {name: (x, y) for name, typ, status, x, y in nodes}

status_colors = {
    'ASSOCIATED': 'dodgerblue',
    'ASSOCIATION_PENDING': 'orange',
    'WAITING_FOR_SELECTED_FT': 'yellow',
    'ASSOCIATION_PREPARING': 'pink',
    'NOT_ASSOCIATED': 'gray',
    'UNKNOWN': 'black'
}

colors = [
    'red' if G.nodes[n]['type'] == 'FT'
    else status_colors.get(G.nodes[n].get('status', 'UNKNOWN'), 'black')
    for n in G.nodes
]

plt.figure(figsize=(10, 6))
nx.draw(G, pos, with_labels=True, node_color=colors, node_size=600, font_size=8, edge_color='gray')
plt.title('DECT-2020 NR Topologie mit FT-PT Verbindungen')
plt.axis('equal')
plt.grid(True)
plt.show()
)";
    topo.close();

    for(auto& rdidName : longRdidToName)
    {
        NS_LOG_UNCOND("Name: " << rdidName.second << " -->  LongRDID: 0x" << std::hex << rdidName.first);
    }

    Simulator::Destroy();
    return 0;
}

// int main(int argc, char* argv[])
// {
//     Simulator::Stop(Seconds(30));

//     LogComponentEnable("Dect2020NetDevice", LOG_LEVEL_INFO);
//     LogComponentEnable("Dect2020Mac", LOG_LEVEL_INFO);
//     LogComponentEnable("Dect2020Phy", LOG_LEVEL_INFO);

//     Dect2020ChannelManager channelManager;
//     channelManager.InitializeChannels(1, 1); // Band 1, Scaling Factor 1

//     // 2 FTs + 10 PTs
//     NodeContainer ftNodes;
//     ftNodes.Create(2);
//     NodeContainer ptNodes;
//     ptNodes.Create(50);

//     MobilityHelper ftMobility;
//     Ptr<ListPositionAllocator> ftPos = CreateObject<ListPositionAllocator>();
//     ftPos->Add(Vector(0.0, 0.0, 0.0));
//     ftPos->Add(Vector(100.0, 0.0, 0.0));
//     ftMobility.SetPositionAllocator(ftPos);
//     ftMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
//     ftMobility.Install(ftNodes);

//     MobilityHelper ptMobility;
//     ptMobility.SetPositionAllocator("ns3::GridPositionAllocator",
//         "MinX", DoubleValue(10.0),
//         "MinY", DoubleValue(10.0),
//         "DeltaX", DoubleValue(10.0),
//         "DeltaY", DoubleValue(10.0),
//         "GridWidth", UintegerValue(5),
//         "LayoutType", StringValue("RowFirst"));
//     ptMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
//     ptMobility.Install(ptNodes);

//     // Kanalmodell
//     Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
//     Ptr<ConstantSpeedPropagationDelayModel> delayModel =
//     CreateObject<ConstantSpeedPropagationDelayModel>(); Ptr<LogDistancePropagationLossModel>
//     lossModel = CreateObject<LogDistancePropagationLossModel>();
//     channel->SetPropagationDelayModel(delayModel);
//     channel->AddPropagationLossModel(lossModel);

//     NetDeviceContainer devices;

//     // FT-Devices
//     for (uint32_t i = 0; i < ftNodes.GetN(); ++i)
//     {
//         Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();
//         dev->SetBandNumber(1);
//         dev->SetTerminationPointType(TermPointType::FT);

//         Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac>();
//         Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy>();

//         dev->SetMac(mac);
//         dev->SetPhy(phy);
//         mac->SetNetDevice(dev);
//         mac->SetPhy(phy);
//         phy->SetMac(mac);
//         ftNodes.Get(i)->AddDevice(dev);
//         phy->SetDevice(dev);
//         phy->SetMobility(ftNodes.Get(i)->GetObject<MobilityModel>());
//         phy->SetChannel(channel);
//         channel->AddRx(phy);

//         dev->SetAddress(Mac48Address::Allocate());
//         dev->SetLinkUp();
//         devices.Add(dev);
//         phy->Start();
//         mac->Start();
//     }

//     // PT-Devices
//     for (uint32_t i = 0; i < ptNodes.GetN(); ++i)
//     {
//         Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();
//         dev->SetBandNumber(1);
//         dev->SetTerminationPointType(TermPointType::PT);

//         Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac>();
//         Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy>();

//         dev->SetMac(mac);
//         dev->SetPhy(phy);
//         mac->SetNetDevice(dev);
//         mac->SetPhy(phy);
//         phy->SetMac(mac);
//         ptNodes.Get(i)->AddDevice(dev);
//         phy->SetDevice(dev);
//         phy->SetMobility(ptNodes.Get(i)->GetObject<MobilityModel>());
//         phy->SetChannel(channel);
//         channel->AddRx(phy);

//         dev->SetAddress(Mac48Address::Allocate());
//         dev->SetLinkUp();
//         devices.Add(dev);
//         phy->Start();
//         mac->Start();
//     }

//     Simulator::Run();

//         std::map<std::string, uint32_t> statusCounter;
//     for (uint32_t i = 0; i < devices.GetN(); ++i)
//     {
//         Ptr<Dect2020NetDevice> dev = DynamicCast<Dect2020NetDevice>(devices.Get(i));
//         Ptr<Dect2020Mac> mac = dev->GetMac();
//         auto status = mac->GetAssociationStatus();
//         std::string statusStr;
//         switch (status)
//         {
//             case 0: statusStr = "NOT_ASSOCIATED"; break;
//             case 1: statusStr = "ASSOCIATION_PREPARING"; break;
//             case 2: statusStr = "WAITING_FOR_SELECTED_FT"; break;
//             case 3: statusStr = "ASSOCIATION_PENDING"; break;
//             case 4: statusStr = "ASSOCIATED"; break;
//             default: statusStr = "UNKNOWN"; break;
//         }
//         statusCounter[statusStr]++;
//         NS_LOG_UNCOND("Device " << i << " LongRDID: 0x" << std::hex <<mac->GetLongRadioDeviceId()
//                          << " --> Association Status: " << statusStr);
//     }

//     NS_LOG_UNCOND("\nSummary of association statuses:");
//     for (const auto& entry : statusCounter)
//     {
//         NS_LOG_UNCOND(entry.first << ": " << entry.second);
//     }

//     Simulator::Destroy();
//     return 0;
// }

// int main(int argc, char* argv[])
// {
//     Simulator::Stop(Seconds(30));

//     LogComponentEnable("Dect2020NetDevice", LOG_LEVEL_INFO);
//     LogComponentEnable("Dect2020Mac", LOG_LEVEL_INFO);
//     LogComponentEnable("Dect2020Phy", LOG_LEVEL_INFO);

//     Dect2020ChannelManager channelManager;
//     channelManager.InitializeChannels(1, 1); // Band 1, Scaling Factor 1

//     NodeContainer nodes;
//     nodes.Create(10);

//     MobilityHelper mobility;
//     mobility.SetPositionAllocator("ns3::GridPositionAllocator",
//         "MinX", DoubleValue(0.0),
//         "MinY", DoubleValue(0.0),
//         "DeltaX", DoubleValue(5.0),
//         "DeltaY", DoubleValue(5.0),
//         "GridWidth", UintegerValue(5),
//         "LayoutType", StringValue("RowFirst"));
//     mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
//     mobility.Install(nodes);

//     Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
//     Ptr<ConstantSpeedPropagationDelayModel> delayModel =
//     CreateObject<ConstantSpeedPropagationDelayModel>(); Ptr<LogDistancePropagationLossModel>
//     lossModel = CreateObject<LogDistancePropagationLossModel>();
//     channel->SetPropagationDelayModel(delayModel);
//     channel->AddPropagationLossModel(lossModel);

//     NetDeviceContainer devices;
//     for (uint32_t i = 0; i < nodes.GetN(); ++i)
//     {
//         Ptr<Dect2020NetDevice> dev = CreateObject<Dect2020NetDevice>();
//         dev->SetBandNumber(1);

//         dev->SetTerminationPointType(i == 0 ? TermPointType::FT : TermPointType::PT);

//         Ptr<Dect2020Mac> mac = CreateObject<Dect2020Mac>();
//         Ptr<Dect2020Phy> phy = CreateObject<Dect2020Phy>();

//         dev->SetMac(mac);
//         dev->SetPhy(phy);
//         mac->SetNetDevice(dev);
//         mac->SetPhy(phy);
//         phy->SetMac(mac);
//         phy->SetDevice(dev);
//         phy->SetChannel(channel);

//         nodes.Get(i)->AddDevice(dev);
//         dev->SetAddress(Mac48Address::Allocate());
//         dev->SetLinkUp();

//         phy->SetMobility(nodes.Get(i)->GetObject<MobilityModel>());

//         devices.Add(dev);
//         phy->Start();
//         mac->Start();
//     }

//     Simulator::Run();
//     Simulator::Destroy();
//     return 0;
// }
