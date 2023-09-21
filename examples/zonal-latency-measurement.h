#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/stream-latency-trace-helper.h"

/**
 * \defgroup zonal-research Module containing code for automotive zonal research.
 */

using namespace::ns3;

typedef std::vector<int> Topology;

const DataRate DATARATE_100BASET("100Mbps");
const DataRate DATARATE_1000BASET("1000Mbps");

const Time PROPAGATION_DELAY("2ns");

const DataRate ENDPOINT_DATARATE = DATARATE_100BASET;
const DataRate BACKBONE_DATARATE = DATARATE_1000BASET;

const DataRate ZONAL_CONTROLLER_PROCESSING_SPEED("1000Mbps");
const QueueSize ZONAL_CONTROLLER_INPUT_QUEUE_SIZE("100p");
const Time ZONAL_CONTROLLER_PROCESSING_DELAY("30ns");

const DataRate MACSEC_TRX_PROCESSING_SPEED("1000Mbps");
const QueueSize MACSEC_TRX_INPUT_QUEUE_SIZE("100p");
// MACSEC_TRX_PROCESSING_DELAY is set as a callback to 
// `ComputePenaMacSecLatency`, which uses a linear regression of 
// the latency data from Pena et al. to guess the latency
// of the MARVELL board they used.

const Topology ZONE_COUNTS = { 6, 3, 8, 6 };


class MySimulation {

public:
    MySimulation(bool verbose, bool use_drop, ns3::Time timeout);

    void Setup();
    int NumZones();

    void PrintNodeInfo();

private:
    bool verbose;
    bool use_drop;
    ns3::Time timeout;

    CsmaHelper csmaHelper;
    
    // The last element of `zoneSwitchDevices` contains the
    // device looking towards the gateway.
    std::vector<NetDeviceContainer> zoneSwitchDevices;
    std::vector<NetDeviceContainer> zoneSwitchMacSecTrxDevices;
    std::vector<NetDeviceContainer> zoneGwMacSecTrxDevices;
    std::vector<NetDeviceContainer> zoneTerminalDevices;
    NetDeviceContainer gatewayDevices;

    // A vector of containers that each reference all netdevices
    // on each zone that should have an Ip. This includes all
    // terminal net devices and the corresponding net device
    // on the central gateway, but _not_ any devices on the switch.
    //
    // All devices in this structure are previously referenced,
    // but this is useful for getting devices by subnet.
    std::vector<NetDeviceContainer> zoneIpDevices;
    std::vector<Ipv4InterfaceContainer> zoneIpInterfaces;

    std::vector<NodeContainer> zoneSwitches;
    std::vector<NodeContainer> zoneSwitchMacSecTrxs;
    std::vector<NodeContainer> zoneGwMacSecTrxs;
    std::vector<NodeContainer> zoneTerminals;
    NodeContainer gateway;

    Topology zone_counts = ZONE_COUNTS;

    void CreateNodes();

    void BuildTopology();
    void BuildZoneTopos();
    void BuildBackboneTopo();
    void BuildSwitches();

    void BuildZonalSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices);
    void BuildBridgeSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices);
    void BuildOpenFlowSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices);
    void BuildMacSecTrx(Ptr<Node> & trxNode, NetDeviceContainer & switchDevices);

    void AssignIpAddresses();

    void CreateApplications();

    void ConfigureTracing();

};
