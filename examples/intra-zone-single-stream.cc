/**
 * \file 
 * \ingroup zonal-research
 *
 * Implementation for the IntraZoneSingleStream simulation.
 *
 * Uses a basic ZonalLayoutHelper layout and the default settings.
 * Simulates just a single stream of traffic from one node to one
 * in the same zone.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/network-module.h"
#include "ns3/bridge-module.h"

#include "ns3/zonal-research-module.h"
#include "ns3/zonal-layout-helper.h"
#include "ns3/processing-bridge-helper.h"
#include "ns3/processing-bridge-net-device.h"
#include "ns3/stream-trace-helper.h"

#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <istream>
#include <ostream>

using namespace ns3;

// Used to control version of zonal layout configuration;
// when the configuration schema changes, so does the underlying
// typename. This forces us to update the configuration 
// for this file each time we change the configuration schema.
typedef ZonalLayoutConfigurationV1 ZonalLayoutConfiguration;

NS_LOG_COMPONENT_DEFINE("IntraZoneSingleStream");


// === Parameters =================================================================

bool insecure = false; //!< Set by CLI argument
bool verbose = false; //!< Set by CLI argument
uint32_t packetSize = 512; //!< Set by CLI argument
DataRate sendingDataRate = DataRate("500Kbps"); //!< Set by CLI argument

ZonalLayoutConfiguration
GetConfig()
{
    ZonalLayoutConfiguration config;

    config.title = "intra-zone-single-stream";

    config.propagationDelay = Time("2ns");

    config.endpointDataRate = DataRate("100Mbps");
    config.backboneDataRate = DataRate("1000Mbps");

    config.zonalControllerProcessingSpeed = DataRate("1000Mbps");
    config.zonalControllerInputQueueSize = QueueSize("100p");
    config.zonalControllerProcessingDelay = Time("30ns");

    config.macsecTrxProcessingSpeed = DataRate("1000Mbps");
    config.macsecTrxInputQueueSize = QueueSize("100p");
    config.macsecTrxProcessingDelay = 
        LatencyCallback(MakeCallback(ZonalLayoutHelper::ComputePenaMacSecLatency));

    config.zoneCounts = {6, 3, 8, 6};

    config.tracing = false;
    config.includeSecurityAdditions = !insecure;

    return config;
}


// === Setup Helpers ==============================================================

/**
 * Create a single UDP stream that we track. 
 *
 * Sends from 10.1.1.3 to 10.1.1.2 (within the same zone).
 */
void
CreateApplications(ZonalLayoutHelper & zonal)
{
    NS_LOG_FUNCTION_NOARGS();

    uint16_t port = 9; // Discard port (RFC 863)

    ApplicationContainer app;

    // Create the sender (zone 1, node 1: 10.1.1.3)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("10.1.1.2"), port)));
    onoff.SetConstantRate(sendingDataRate, packetSize);

    app = onoff.Install(zonal.GetNode(0, 1));
    app.Start(Seconds(1.1));
    app.Stop(Seconds(10.0));
    Ptr<OnOffApplication> sendingApp = DynamicCast<OnOffApplication>(app.Get(0));

    // Create the receiver (zone 0, node 0: 10.1.1.2)
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    app = sink.Install(zonal.GetNode(0, 0));
    app.Start(Seconds(0.0));
    Ptr<PacketSink> receivingApp = DynamicCast<PacketSink>(app.Get(0));

    // We start tracing a little after the application starts sending to avoid
    // getting any delays or effects from the ARP requests. Those are useful to know about,
    // but not what we are studying here.
    zonal.AddStreamTraceHelper(sendingApp, receivingApp, "stream", Seconds(1.5));
}


// === Command Line Callbacks =====================================================

bool
SetVerbose(const std::string& value)
{
    verbose = true;
    return true;
}

void
ParseCommandLine(int argc, char *argv[])
{
    NS_LOG_FUNCTION_NOARGS();
    CommandLine cmd(__FILE__);
    cmd.AddValue("v", "Verbose (turns on logging).", MakeCallback(&SetVerbose));
    cmd.AddValue("verbose", "Verbose (turns on logging).", MakeCallback(&SetVerbose));
    cmd.AddValue("packet-size", "Size of packets in bytes", packetSize);
    cmd.AddValue("insecure", "Turns off security additions for a theoretically faster network"
                             " baseline", insecure);
    cmd.AddValue("sending-rate", "How fast the sender sends data", sendingDataRate);
    cmd.Parse(argc, argv);
}


// === Main =======================================================================

int
main(int argc, char* argv[])
{
    ParseCommandLine(argc, argv);

    ZonalLayoutConfiguration config {GetConfig()};
    ZonalLayoutHelper zonal(config);

    if (verbose)
    {
        LogComponentEnable("ZonalLayoutHelper", LOG_LEVEL_ALL);
        LogComponentEnable("IntraZoneSingleStream", LOG_LEVEL_ALL);
        LogComponentEnable("ProcessingBridgeNetDevice", LOG_LEVEL_ALL);
        // LogComponentEnable("OnOffApplication", LOG_LEVEL_ALL);
        // LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
        // LogComponentEnable("Packet", LOG_LEVEL_ALL);
        // LogComponentEnable("DropTailQueue", LOG_LEVEL_ALL);
        // LogComponentEnable("Queue", LOG_LEVEL_ALL);
    }

    Packet::EnablePrinting();
    Packet::EnableChecking();
    zonal.Setup();

    CreateApplications(zonal);

    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();

    zonal.End();
    NS_LOG_INFO("Done.");

    return 0;
}
