/**
 * \file 
 * \ingroup zonal-research
 *
 * Implementation for the ZonalLayoutHelper for simulation.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/network-module.h"
#include "ns3/bridge-module.h"

#include "zonal-layout-helper.h"
#include "ns3/processing-bridge-helper.h"
#include "ns3/processing-bridge-net-device.h"
#include "ns3/stream-latency-trace-helper.h"

#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <ostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ZonalLayoutHelper");


// === Setup Helpers ==============================================================

ZonalLayoutHelper::ZonalLayoutHelper(ZonalLayoutConfiguration config) :
    config(config)
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
ZonalLayoutHelper::PrintNodeInfo() const
{
    NS_LOG_FUNCTION(this);

    std::ofstream out(config.title + ".ips");

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
ZonalLayoutHelper::Setup()
{
    NS_LOG_FUNCTION(this);

    CreateNodes();
    BuildTopology();
    AssignIpAddresses();
    if (config.tracing) 
    {
        ConfigureTracing();
    }
    PrintNodeInfo();
}

int 
ZonalLayoutHelper::NumZones() const
{
    return config.zoneCounts.size();
}

NodeContainer
ZonalLayoutHelper::GetNode(int zone, int node)
{
    return zoneTerminals[zone].Get(node);
}

void
ZonalLayoutHelper::CreateNodes()
{
    NS_LOG_FUNCTION(this);
    
    gateway.Create(1);

    for (int zone = 0; zone < NumZones(); zone++) {
        zoneTerminals[zone].Create(config.zoneCounts[zone]);
        zoneSwitches[zone].Create(1);
        zoneSwitchMacSecTrxs[zone].Create(1);
        zoneGwMacSecTrxs[zone].Create(1);
    }
}

void
ZonalLayoutHelper::BuildZoneTopos()
{
    NS_LOG_FUNCTION(this);

    csmaHelper.SetChannelAttribute("Delay", TimeValue(config.propagationDelay));
    csmaHelper.SetChannelAttribute("DataRate", DataRateValue(config.endpointDataRate));

    // Create p2p links from each terminal to their zonal switch
    for (int zone = 0; zone < NumZones(); zone++) {
        for (int terminal = 0; terminal < config.zoneCounts[zone]; terminal++) {
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
ZonalLayoutHelper::BuildBackboneTopo()
{
    NS_LOG_FUNCTION(this);

    csmaHelper.SetChannelAttribute("Delay", TimeValue(config.propagationDelay));
    csmaHelper.SetChannelAttribute("DataRate", DataRateValue(config.backboneDataRate));

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
ZonalLayoutHelper::BuildSwitches()
{
    // Create the switch netdevices, which will do the packet switching
    // Also create the MacSecTrxs
    for (int zone = 0; zone < NumZones(); zone++) {
        Ptr<Node> switchNode = zoneSwitches[zone].Get(0);
        BuildZonalSwitch(switchNode, zoneSwitchDevices[zone]);

        // MacSecTrxs
        Ptr<Node> switchMacSecTrx = zoneSwitchMacSecTrxs[zone].Get(0);
        Ptr<Node> gwMacSecTrx = zoneGwMacSecTrxs[zone].Get(0);

        BuildMacSecTrx(switchMacSecTrx, zoneSwitchMacSecTrxDevices[zone]);
        BuildMacSecTrx(gwMacSecTrx, zoneGwMacSecTrxDevices[zone]);
    }
}

void
ZonalLayoutHelper::BuildZonalSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices) const
{
    ProcessingBridgeHelper pBridge;
    Time processingDelay = config.zonalControllerProcessingDelay;
    pBridge.SetDeviceAttribute("ProcessingDelay", 
                               CallbackValue(ProcessingBridgeNetDevice::LatencyCallback(
                                    [=] (uint32_t size) { return processingDelay; }
                               )));
    pBridge.SetDeviceAttribute("Throughput",
                               DataRateValue(config.zonalControllerProcessingSpeed));
    pBridge.SetDeviceAttribute("QueueSize",
                               QueueSizeValue(config.zonalControllerInputQueueSize));

    pBridge.Install(switchNode, switchDevices);
}

Time 
ZonalLayoutHelper::ComputePenaMacSecLatency(uint32_t packet_size_bytes) {
    // Equation acquired from doing linear regression on Pena et al.
    // latency data in paper
    //
    // Divide by two because their data includes encryption & decryption, whereas
    // we only want to know how long it takes to do one of those things.
    return NanoSeconds(((uint64_t)packet_size_bytes * 32 + 6579) / 2);
}

void
ZonalLayoutHelper::BuildMacSecTrx(Ptr<Node> & trxNode, NetDeviceContainer & switchDevices) const
{
    ProcessingBridgeHelper macSecTrx;
    macSecTrx.SetDeviceAttribute("ProcessingDelay",
                                 CallbackValue(ProcessingBridgeNetDevice::LatencyCallback(
                                    ComputePenaMacSecLatency
                                 )));
    macSecTrx.SetDeviceAttribute("Throughput",
                               DataRateValue(config.macsecTrxProcessingSpeed));
    macSecTrx.SetDeviceAttribute("QueueSize",
                               QueueSizeValue(config.macsecTrxInputQueueSize));
    macSecTrx.Install(trxNode, switchDevices);
}

void
ZonalLayoutHelper::BuildBridgeSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices)
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
ZonalLayoutHelper::BuildTopology()
{
    NS_LOG_FUNCTION(this);

    BuildZoneTopos();
    BuildBackboneTopo();
    BuildSwitches();
}

void
ZonalLayoutHelper::AssignIpAddresses()
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
    Ptr<OutputStreamWrapper> routingStream = 
        Create<OutputStreamWrapper>(config.title + ".routes", std::ios::out);
    Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(0.5), routingStream);
}

void
ZonalLayoutHelper::ConfigureTracing()
{
    NS_LOG_FUNCTION(this);

    //
    // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
    // Trace output will be sent to the file "openflow-switch.tr"
    //
    AsciiTraceHelper ascii;
    csmaHelper.EnableAsciiAll(ascii.CreateFileStream(config.title + ".tr"));

    //
    // Also configure some tcpdump traces; each interface will be traced.
    // The output files will be named:
    //     openflow-switch-<nodeId>-<interfaceId>.pcap
    // and can be read by the "tcpdump -r" command (use "-tt" option to
    // display timestamps correctly)
    //
    csmaHelper.EnablePcapAll(config.title, true);
}

