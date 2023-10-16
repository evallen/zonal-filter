/**
 * \file
 * \ingroup zonal-research
 *
 * Based off of the BridgeHelper.
 * Contains the declaration of the ProcessingBridgeHelper class.
 */
#ifndef PROCESSING_BRIDGE_HELPER_H
#define PROCESSING_BRIDGE_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/object-factory.h"

#include <string>

namespace ns3
{

class Node;
class AttributeValue;

/**
 * \ingroup zonal-research
 *
 * Helper to create a ProcessingBridgeNetDevice that connects a 
 * set of NetDevices in a NetDeviceContainer on the same LAN segment.
 *
 * Use like any other |ns3| helper - use ``Install`` on the net devices
 * you want.
 */
class ProcessingBridgeHelper
{
  public:
    ProcessingBridgeHelper();

    /**
     * Set an attribute on each ns3::ProcessingBridgeNetDevice created by
     * ProcessingBridgeHelper::Install
     *
     * \param n1 the name of the attribute to set
     * \param v1 the value of the attribute to set
     */
    void SetDeviceAttribute(std::string n1, const AttributeValue& v1);

    /**
     * This method creates an ns3::ProcessingBridgeNetDevice with the attributes
     * configured by ProcessingBridgeHelper::SetDeviceAttribute, adds the device
     * to the node, and attaches the given NetDevices as ports of the
     * bridge.
     *
     * \param node The node to install the device in
     * \param c Container of NetDevices to add as zonal bridge ports
     * \param name The name of the bridge
     * \returns A container holding the added net device.
     */
    NetDeviceContainer Install(Ptr<Node> node, NetDeviceContainer c, std::string name);

    /**
     * This method creates an ns3::ProcessingBridgeNetDevice with the attributes
     * configured by ProcessingBridgeHelper::SetDeviceAttribute, adds the device
     * to the node, and attaches the given NetDevices as ports of the
     * zonal bridge.
     *
     * \param nodeName The name of the node to install the device in
     * \param c Container of NetDevices to add as zonal bridge ports
     * \param name The name of the bridge
     * \returns A container holding the added net device.
     */
    NetDeviceContainer Install(std::string nodeName, NetDeviceContainer c, std::string name);

  private:
    ObjectFactory m_deviceFactory; //!< Object factory
};

} // namespace ns3

#endif /* PROCESSING_BRIDGE_HELPER_H */
