/**
 * \file
 * \ingroup zonal-research
 *
 * Declaration of a ZonalLayoutHelper.
 *
 * See the class documentation of ZonalLayoutHelper below
 * for information on the zonal architecture.
 */
#ifndef ZONAL_LAYOUT_HELPER_H
#define ZONAL_LAYOUT_HELPER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/stream-latency-trace-helper.h"
#include "ns3/processing-bridge-net-device.h"

namespace ns3
{

using LatencyCallback = ProcessingBridgeNetDevice::LatencyCallback;

/** 
 * \ingroup zonal-research
 *
 * Represents the zonal topology. Each int represents the number of
 * endpoints in each zone, so { 6, 3, 8, 6 } is a four-zone network
 * where the zones have 6, 3, 8, and 6 endpoints, respectively.
 */
typedef std::vector<int> Topology;


/**
 * \ingroup zonal-research
 *
 * Struct to hold basic configuration information for a zonal topology.
 */
struct ZonalLayoutConfiguration 
{
    /** The title of any files that we create. */
    std::string title;
    
    /** 
     * The time it takes to propagate a bit along a connection.
     * For simiplicity, we assume that each wire is the same length
     * (though we can make this more detailed later too).
     */
    Time propagationDelay;

    /**
     * The datarate of the connections between switch and 
     * ECU endpoints.
     */
    DataRate endpointDataRate;

    /**
     * The datarate of the connections between gateway and 
     * switches.
     */
    DataRate backboneDataRate;

    /**
     * The datarate that the zonal controller processes
     * packets at, _assuming separate processes 
     * per input_. In other words, if the zonal controller
     * processing speed is 1000Mbps, it can process 1000Mbps
     * of packets from _each_ input simultaneously.
     *
     * This may not be accurate to reality and can be the 
     * subject of further investigation.
     */
    DataRate zonalControllerProcessingSpeed;

    /**
     * The size of each input queue on the zonal controller.
     */
    QueueSize zonalControllerInputQueueSize;

    /**
     * The time it takes for the zonal controller to process
     * each incoming packet. This is differnet from the processing
     * speed (i.e., data rate) because it affects how long it takes
     * a _single_ packet to go through the zonal controller, whereas
     * the processing speed affects the amount of time before the next
     * packet can begin processing.
     */
    Time zonalControllerProcessingDelay;

    /**
     * The data rate (throughput)
     * that the MACsec transceivers process packets at,
     * which is considered separate in both directions.
     */
    DataRate macsecTrxProcessingSpeed;

    /**
     * The size of the input queues on the MACsec transceivers.
     */
    QueueSize macsecTrxInputQueueSize;

    /**
     * The callback to determine how long it takes the MACsec transceiver
     * to process a given packet given its size.
     *
     * Right now, the one you want is probably the function
     * ZonalLayoutHelper::ComputePenaMacSecLatency function, since that 
     * is based on a linear regression of the data from Pena et al.'s measurements
     * of a MARVELL 88E1680M board.
     */
    LatencyCallback macsecTrxProcessingDelay;

    /** Actual zone topoology for the simulation. */
    Topology zoneCounts;
};


/**
 * \ingroup zonal-research
 *
 * Class to create a zonal network topology with configurable settings.
 *
 * For details on the network architecture, see the documentation
 * at the top of this file ``zonal-layout-helper.h``.
 *
 * Network topology: 
 *  One or more zones with one or more endpoints each
 *  connected to a zonal gateway in a tree-like structure.
 *
 * For example, a zonal network with topology = {2, 3}
 *  (two zones, one with 2 endpoints and one with 3) would look
 *  like this:
 *
 *         ZONE ONE                       |                ZONE TWO
 *                                        |
 *    n1.1 ----|                          |                          |---- n2.1
 *             |                                                     |
 *            n1.0 == n1.M ===== n1.N == nGW == n2.N ===== n2.M -- n2.0 -- n2.2
 *             |                                                     |
 *    n1.2 ----|                          |                          |---- n2.3
 *
 *    where nodes are represented as:
 *      nGW     - is the gateway
 *      n1.*    - is the zone-one nodes (n1.1)
 *      n2.*    - is the zone-two nodes (n2.1)
 *      n*.M    - is the MACsec transceiver for a zonal controller (n1.M)
 *      n*.N    - is the MACsec transceiver for the gateway on a given zone (n1.N) (NOTE*)
 *      n*.0    - is the zonal controller for a zone (n1.0)
 *
 *    and where connections are respresented as:
 *      ---     - "Endpoint" Automotive Ethernet (e.g., 100Mbps)
 *      ===     - "Backbone" Automotive Ethernet (e.g., 1000Mbps)
 *
 *    Depending on the topology specified in the header file,
 *    the simulation could have more zones with different numbers
 *    of endpoints, etc.
 *
 *    NOTE*: 
 *      Currently, we simulate our network with a MACsec transceiver on each side
 *      of each backbone connection (i.e., an inter-zonal message gets encrypted
 *      when leaving a zonal controller, decrypted when entering the gateway, encrypted
 *      when leaving the gateway, and then decrypted again when entering a zonal
 *      controller). 
 *
 *      It is possible that we could implement the MACsec transceivers so that 
 *      we only have one per zone (such as one on each ZC, and allow the messsages
 *      to go through the GW encrypted). 
 *
 *      This should be the topic of a future simulation, potentially implemented 
 *      with a switch in the configuration.
 */
class ZonalLayoutHelper 
{

public:
    /** Create a new simulation object.
     *
     * \param verbose Whether or not to print extra logging information.
     */
    ZonalLayoutHelper(ZonalLayoutConfiguration config);

    /** Construct the simuluation. */
    void Setup();

    /** 
     * Get the number of zones in the sinmulation. 
     * \return The number of zones.
     */
    int NumZones() const;

    /**
     * Get a reference to the endpoint node in the specified zone.
     *
     * \param zone The zone to get a node from.
     * \param node The index of the node in that zone, starting at 0.
     * 
     * \return The specified node in a NodeContainer.
     */
    NodeContainer GetNode(int zone, int node);

    /** 
     * Print the IPs of all the nodes with their node IDs 
     * to a file 'zonal-latency-measurement.ips'.
     */
    void PrintNodeInfo() const;

    /**
     * Compute the expected latency of a packet passing through a
     * MARVELL M88E1680M board based on a linear regression
     * of the data in Pena et al.'s paper. 
     */
    static Time ComputePenaMacSecLatency(uint32_t packet_size_bytes);

private:
    ZonalLayoutConfiguration config;

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

    void CreateNodes();

    void BuildTopology();
    void BuildZoneTopos();
    void BuildBackboneTopo();
    void BuildSwitches();

    void BuildZonalSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices) const;
    void BuildBridgeSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices);
    void BuildOpenFlowSwitch(Ptr<Node> & switchNode, NetDeviceContainer & switchDevices);
    void BuildMacSecTrx(Ptr<Node> & trxNode, NetDeviceContainer & switchDevices) const;

    void AssignIpAddresses();

    void ConfigureTracing();

};

}

#endif /* ZONAL_LAYOUT_HELPER_H */
