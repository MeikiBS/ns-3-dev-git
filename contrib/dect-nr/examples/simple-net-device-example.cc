#include "ns3/core-module.h"
#include "ns3/dect-channel.h"
#include "ns3/dect-nr-helper.h"
#include "ns3/end-device-dect-nr-mac.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/simple-net-device-helper.h"
#include "ns3/simple-channel.h"

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

    // Erstelle einen Kanal
    Ptr<SimpleChannel> channel = CreateObject<SimpleChannel>();

    // Erstelle ein TX Objekt
    SimpleNetDeviceHelper helper = SimpleNetDeviceHelper();
    NodeContainer nodes(2);
    helper.SetChannelAttribute("Channel", PointerValue(channel));
    helper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    helper.Install(nodes);






    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
