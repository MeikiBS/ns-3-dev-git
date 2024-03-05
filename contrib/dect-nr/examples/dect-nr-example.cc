#include "ns3/core-module.h"
#include "ns3/dect-nr-helper.h"
#include "ns3/log.h"
#include "ns3/dect-channel.h"
#include "ns3/node-container.h"

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
    //Ptr<Dect2020Channel> channel = CreateObject<Dect2020Channel>();

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}

