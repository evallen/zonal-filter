/* 
 * mysimulation.cc
 *
 * A basic model of the ZIA network for simulation.
 *
 */

// Network topology
// An arbitrary zonal network with some number of zones and some
// number of nodes in each zone. The topology is defined by
// the ZONE_COUNTS constant in mysimulation.h. 

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/network-module.h"
#include "ns3/bridge-module.h"

#include "ns3/macsec-trx-helper.h"
#include "zonal-latency-measurement.h"
#include "ns3/processing-bridge-helper.h"
#include "ns3/processing-bridge-net-device.h"
#include "ns3/stream-latency-trace-helper.h"

#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <ostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ZonalNetwork");


// === Parameters =================================================================

bool verbose = false;
bool use_drop = false;
ns3::Time timeout = ns3::Seconds(0);


// === Command Line Callbacks =====================================================

bool
SetVerbose(const std::string& value)
{
    verbose = true;
    return true;
}

bool
SetDrop(const std::string& value)
{
    use_drop = true;
    return true;
}

bool
SetTimeout(const std::string& value)
{
    try
    {
        timeout = ns3::Seconds(std::stof(value));
        return true;
    }
    catch (...)
    {
        return false;
    }
    return false;
}


// === Setup Helpers ==============================================================

void
ParseCommandLine(int argc, char *argv[])
{
    NS_LOG_FUNCTION_NOARGS();
    CommandLine cmd(__FILE__);
    cmd.AddValue("v", "Verbose (turns on logging).", MakeCallback(&SetVerbose));
    cmd.AddValue("verbose", "Verbose (turns on logging).", MakeCallback(&SetVerbose));
    cmd.AddValue("d", "Use Drop Controller (Learning if not specified).", MakeCallback(&SetDrop));
    cmd.AddValue("drop",
                 "Use Drop Controller (Learning if not specified).",
                 MakeCallback(&SetDrop));
    cmd.AddValue("t",
                 "Learning Controller Timeout (has no effect if drop controller is specified).",
                 MakeCallback(&SetTimeout));
    cmd.AddValue("timeout",
                 "Learning Controller Timeout (has no effect if drop controller is specified).",
                 MakeCallback(&SetTimeout));

    cmd.Parse(argc, argv);
}

MySimulation::MySimulation(bool verbose, bool use_drop, ns3::Time timeout) :
    verbose(verbose),
    use_drop(use_drop),
    timeout(timeout)
{
    zoneSwitchDevices = std::vector<NetDeviceContainer>(NumZones());
    zoneSwitchMacSecTrxDevices = std::vector<NetDeviceContainer>(NumZones());
    zoneGwMacSecTrxDevices = std::vector<NetDeviceContainer>(NumZones());
    zoneTerminalDevices = std::vector<NetDeviceContainer>(NumZones());

    zoneIpDevices = std::vector<NetDeviceContainer>(NumZones());
    zoneIpInterfaces = std::vector<Ipv4InterfaceContainer>(NumZones());

    zoneSwitches = std::vector<NodeContainer>(NumZones());
    zoneSwitchMacSecTrxs = std::vector<NodeContainer>(NumZones());
    zoneGwMacSecTrxs = std::vector<NodeContainer>(NumZones());
    zoneTerminals = std::vector<NodeContainer>(NumZones());
}

void 
PrintTerminal(Ptr<Node> node, int zone, int term, std::ofstream & out)
{
    out << "\tNode " << node->GetId() << ": Zone " << zone << "; Term " << term << "; IP " << 
                 node->GetObject<Ipv4>()->GetAddress(1,0).GetLocal() << "\n";
}

void
PrintSwitch(Ptr<Node> node, int zone, std::ofstream & out)
{
    out << "\tNode " << node->GetId() << ": Zone " << zone << "; SWITCH" << "\n";
}

void
PrintMacSecTrx(Ptr<Node> node, int zone, bool switchSide, std::ofstream & out)
{
    std::string suffix = switchSide ? "SWITCH-SIDE" : "GATEWAY-SIDE";
    out << "\tNode " << node->GetId() << ": Zone " << zone << "; " << suffix << "\n";
}

void
PrintGateway(Ptr<Node> node, std::ofstream & out)
{
    Ptr<Ipv4> ip4 = node->GetObject<Ipv4>();
    out << "Node " << node->GetId() << "\n";

    int nInterfaces = ip4->GetNInterfaces();
    for (int i = 0; i < nInterfaces; i++) {
        out << "\tIP " << ip4->GetAddress(i, 0).GetLocal() << "\n";
    }
}

void
MySimulation::PrintNodeInfo()
{
    NS_LOG_FUNCTION(this);

    std::ofstream out("mysimulation.ips");

    out << "GATEWAY" << "\n";
    PrintGateway(gateway.Get(0), out);

    for (int i = 0; i < NumZones(); i++) {
        int numTerminals = zoneTerminals[i].GetN();

        out << "ZONE " << i << "\n";
        out << "\tSWITCH" << "\n";
        PrintSwitch(zoneSwitches[i].Get(0), i, out);

        out << "\tMACSEC TRX" << "\n";
        PrintMacSecTrx(zoneSwitchMacSecTrxs[i].Get(0), i, true, out);
        PrintMacSecTrx(zoneGwMacSecTrxs[i].Get(0), i, false, out);

        out << "\tTERMINALS" << "\n";
        for (int j = 0; j < numTerminals; j++) {
            Ptr<Node> node = zoneTerminals[i].Get(j);
            PrintTerminal(node, i, j, out);
        }
    }
}

void
MySimulation::Setup()
{
    NS_LOG_FUNCTION(this);

    CreateNodes();
    BuildTopology();
    AssignIpAddresses();
    CreateApplications();
    ConfigureTracing();
    PrintNodeInfo();
}

int 
MySimulation::NumZones()
{
    return zone_counts.size();
}

void
MySimulation::CreateNodes()
{
    NS_LOG_FUNCTION(this);
    
    gateway.Create(1);

    for (int zone = 0; zone < NumZones(); zone++) {
        zoneTerminals[zone].Create(zone_counts[zone]);
        zoneSwitches[zone].Create(1);
        zoneSwitchMacSecTrxs[zone].Create(1);
        zoneGwMacSecTrxs[zone].Create(1);
    }
}

void
MySimulation::BuildZoneTopos()
{
    NS_LOG_FUNCTION(this);

    csmaHelper.SetChannelAttribute("Delay", TimeValue(PROPAGATION_DELAY));
    csmaHelper.SetChannelAttribute("DataRate", DataRateValue(DATARATE_100BASET));

    // Create p2p links from each terminal to their zonal switch
    for (int zone = 0; zone < NumZones(); zone++) {
        for (int terminal = 0; terminal < zone_counts[zone]; terminal++) {
            NetDeviceContainer link = csmaHelper.Install(
                NodeContainer(
                    zoneTerminals[zone].Get(terminal), zoneSwitches[zone]
                )
            );
            zoneTerminalDevices[zone].Add(link.Get(0));
            zoneSwitchDevices[zone].Add(link.Get(1));
        }
    }

    // Add internet stack to the terminals
    InternetStackHelper internet;
    for (auto & terminals : zoneTerminals) {
        internet.Install(terminals);
    }
}

void
MySimulation::BuildBackboneTopo()
{
    NS_LOG_FUNCTION(this);

    csmaHelper.SetChannelAttribute("Delay", TimeValue(PROPAGATION_DELAY));
    csmaHelper.SetChannelAttribute("DataRate", DataRateValue(DATARATE_1000BASET));

    // Create links
    for (int zone = 0; zone < NumZones(); zone++) {
        // Zonal switch to switch MacSec transceiver
        NetDeviceContainer switch_macsec_link = csmaHelper.Install(
            NodeContainer(
                zoneSwitches[zone], zoneSwitchMacSecTrxs[zone]
            )
        );
        zoneSwitchDevices[zone].Add(switch_macsec_link.Get(0));
        zoneSwitchMacSecTrxDevices[zone].Add(switch_macsec_link.Get(1));

        // Switch MacSec transceiver to gateway MacSec transceiver
        NetDeviceContainer inter_macsec_link = csmaHelper.Install(
            NodeContainer(
                zoneSwitchMacSecTrxs[zone], zoneGwMacSecTrxs[zone]
            )
        );
        zoneSwitchMacSecTrxDevices[zone].Add(inter_macsec_link.Get(0));
        zoneGwMacSecTrxDevices[zone].Add(inter_macsec_link.Get(1));

        // Gateway MacSec transceiver to gateway
        NetDeviceContainer macsec_gw_link = csmaHelper.Install(
            NodeContainer(
                zoneGwMacSecTrxs[zone], gateway
            )
        );
        zoneGwMacSecTrxDevices[zone].Add(macsec_gw_link.Get(0));
        gatewayDevices.Add(macsec_gw_link.Get(1));
    }

    // Add internet stack to the gateway
    InternetStackHelper internet;
    internet.Install(gateway);
}

void
MySimulation::BuildSwitches()
{
    // Create the switch netdevices, which will do the packet switching
    // Also create the MacSecTrxs
    for (int zone = 0; zone < NumZones(); zone++) {
        Ptr<Node> switchNode = zoneSwitches[zone].Get(0);
        // BuildOpenFlowSwitch(switchNode, zoneSwitchDevices[zone]);
        // BuildBridgeSwitch(switchNode, zoneSwitchDevices[zone]);
        BuildZonalSwitch(switchNode, zoneSwitchDevices[zone]);

        // MacSecTrxs
        Ptr<Node> switchMacSecTrx = zoneSwitchMacSecTrxs[zone].Get(0);
        Ptr<Node> gwMacSecTrx = zoneGwMacSecTrxs[zone].Get(0);

        BuildMacSecTrx(switchMacSecTrx, zoneSwitchMacSecTrxDevices[zone]);
        BuildMacSecTrx(gwMacSecTrx, zoneGwMacSecTrxDevices[zone]);
    }
}

void
MySimulation::BuildZonalSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices)
{
    ProcessingBridgeHelper pBridge;
    pBridge.SetDeviceAttribute("ProcessingDelay", 
                               CallbackValue(ProcessingBridgeNetDevice::LatencyCallback(
                                    [] (uint32_t size) { return ZONAL_CONTROLLER_PROCESSING_DELAY; }
                               )));
    pBridge.SetDeviceAttribute("Throughput",
                               DataRateValue(ZONAL_CONTROLLER_PROCESSING_SPEED));
    pBridge.SetDeviceAttribute("QueueSize",
                               QueueSizeValue(ZONAL_CONTROLLER_INPUT_QUEUE_SIZE));

    pBridge.Install(switchNode, switchDevices);
}

Time 
ComputePenaMacSecLatency(uint32_t packet_size_bytes) {
    // Equation acquired from doing linear regression on Pena et al.
    // latency data in paper
    //
    // Divide by two because their data includes encryption & decryption, whereas
    // we only want to know how long it takes to do one of those things.
    return NanoSeconds(((uint64_t)packet_size_bytes * 32 + 6579) / 2);
}

void
MySimulation::BuildMacSecTrx(Ptr<Node> & trxNode, NetDeviceContainer & switchDevices)
{
    ProcessingBridgeHelper macSecTrx;
    macSecTrx.SetDeviceAttribute("ProcessingDelay",
                                 CallbackValue(ProcessingBridgeNetDevice::LatencyCallback(
                                    ComputePenaMacSecLatency
                                 )));
    macSecTrx.SetDeviceAttribute("Throughput",
                               DataRateValue(MACSEC_TRX_PROCESSING_SPEED));
    macSecTrx.SetDeviceAttribute("QueueSize",
                               QueueSizeValue(MACSEC_TRX_INPUT_QUEUE_SIZE));
    macSecTrx.Install(trxNode, switchDevices);
}

void
MySimulation::BuildBridgeSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices)
{
    // NS_LOG_FUNCTION(this << switchNode << switchDevices);
    
    BridgeHelper bridge;

    bridge.Install(switchNode, switchDevices);
}

#ifdef NS3_OPENFLOW
void
MySimulation::BuildOpenFlowSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices)
{
    // NS_LOG_FUNCTION(this << switchNode << switchDevices);

    OpenFlowSwitchHelper swtch;

    if (use_drop)
    {
        Ptr<ns3::ofi::DropController> controller = CreateObject<ns3::ofi::DropController>();
        swtch.Install(switchNode, switchDevices, controller);
    }
    else
    {
        Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController>();
        if (!timeout.IsZero())
        {
            controller->SetAttribute("ExpirationTime", TimeValue(timeout));
        }
        swtch.Install(switchNode, switchDevices, controller);
    }
}
#endif

void
MySimulation::BuildTopology()
{
    NS_LOG_FUNCTION(this);

    BuildZoneTopos();
    BuildBackboneTopo();
    BuildSwitches();
}

void
MySimulation::AssignIpAddresses()
{
    NS_LOG_FUNCTION(this);

    // All zonal networks will be in the format
    //  10.1.X.0, 
    // where X starts at 1 and increases.
    constexpr int subnet_base_num = 1;
    Ipv4AddressHelper ipv4;

    for (int zone = 0; zone < NumZones(); zone++) {
        int subnet_num = zone + subnet_base_num;

        zoneIpDevices[zone].Add(gatewayDevices.Get(zone));
        zoneIpDevices[zone].Add(zoneTerminalDevices[zone]);

        // Create 10.1.X.0 network string
        std::ostringstream network;
        network << "10.1." << subnet_num << ".0";
        std::string network_str = network.str();

        ipv4.SetBase(network_str.c_str(), "255.255.255.0");
        zoneIpInterfaces[zone] = ipv4.Assign(zoneIpDevices[zone]);
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("mysimulation.routes", std::ios::out);
    Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(0.5), routingStream);
}

void
MySimulation::CreateApplications()
{
    NS_LOG_FUNCTION(this);

    uint16_t port = 9; // Discard port (RFC 863)

    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("10.1.1.3"), port)));
    onoff.SetConstantRate(DataRate("500kb/s"));

    ApplicationContainer app;
    // ApplicationContainer app = onoff.Install(zoneTerminals[0].Get(0));
    // // Start the application
    // app.Start(Seconds(1.0));
    // app.Stop(Seconds(10.0));

    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    // app = sink.Install(zoneTerminals[0].Get(1));
    // app.Start(Seconds(0.0));

    //
    // Create a similar flow from n3 to n0, starting at time 1.1 seconds
    //
    onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(Ipv4Address("10.1.1.2"), port)));
    app = onoff.Install(zoneTerminals[1].Get(1));
    app.Start(Seconds(1.1));
    app.Stop(Seconds(10.0));
    Ptr<CsmaNetDevice> sendingDevice = DynamicCast<CsmaNetDevice>(app.Get(0)->GetNode()->GetDevice(0));

    app = sink.Install(zoneTerminals[0].Get(0));
    app.Start(Seconds(0.0));
    Ptr<PacketSink> receivingApp = DynamicCast<PacketSink>(app.Get(0));

    StreamLatencyTraceHelper::Install(sendingDevice, receivingApp, "stream.csv");
}

void
MySimulation::ConfigureTracing()
{
    NS_LOG_FUNCTION(this);

    //
    // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
    // Trace output will be sent to the file "openflow-switch.tr"
    //
    AsciiTraceHelper ascii;
    csmaHelper.EnableAsciiAll(ascii.CreateFileStream("mysimulation.tr"));

    //
    // Also configure some tcpdump traces; each interface will be traced.
    // The output files will be named:
    //     openflow-switch-<nodeId>-<interfaceId>.pcap
    // and can be read by the "tcpdump -r" command (use "-tt" option to
    // display timestamps correctly)
    //
    csmaHelper.EnablePcapAll("mysimulation", true);


}


// === Main =======================================================================

int
main(int argc, char* argv[])
{
    //
    // Allow the user to override any of the defaults and the above Bind() at
    // run-time, via command-line arguments
    //
    ParseCommandLine(argc, argv);
    MySimulation sim(verbose, use_drop, timeout);

    if (verbose)
    {
        LogComponentEnable("ZonalNetwork", LOG_LEVEL_ALL);
        LogComponentEnable("OnOffApplication", LOG_LEVEL_ALL);
        LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
        LogComponentEnable("Packet", LOG_LEVEL_ALL);
        // LogComponentEnable("OpenFlowInterface", LOG_LEVEL_INFO);
        // LogComponentEnable("OpenFlowSwitchNetDevice", LOG_LEVEL_INFO);
    }

    Packet::EnablePrinting();
    Packet::EnableChecking();
    sim.Setup();

    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
