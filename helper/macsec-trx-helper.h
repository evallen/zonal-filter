/**
 * Based off of the BridgeHelper
 */
#ifndef MACSEC_TRX_HELPER_H
#define MACSEC_TRX_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/object-factory.h"
#include "ns3/core-module.h"

#include <string>

namespace ns3
{

class Node;
class AttributeValue;

class MacSecTrxHelper
{
  public:
    MacSecTrxHelper();
    /**
     * Set an attribute on each internal ns3::PointToPointNetDevice created by
     * MacSecTrxHelper::Install
     *
     * \param n1 the name of the attribute to set
     * \param v1 the value of the attribute to set
     */
    void SetDeviceAttribute(std::string n1, const AttributeValue& v1);
    void SetChannelAttribute(std::string n1, const AttributeValue& v1);

    void SetQueue(std::string type, StringValue maxSize);

    /**
     * This method creates an internal ns3::PointToPointNetDevice with the attributes
     * configured by MacSecTrxHelper::SetDeviceAttribute, adds the device
     * to the node, and attaches the given NetDevices as ports of the
     * bridge.
     *
     * \param node The node to install the device in
     * \param c Container of NetDevices to add as MacSec bridge ports
     * \returns A container holding the added net device.
     */
    NetDeviceContainer Install(Ptr<Node> node, Ptr<NetDevice> plain_side,
                               Ptr<NetDevice> secure_side);
    /**
     * This method creates an ns3::ZonalControllerNetDevice with the attributes
     * configured by MacSecTrxHelper::SetDeviceAttribute, adds the device
     * to the node, and attaches the given NetDevices as ports of the
     * zonal bridge.
     *
     * \param nodeName The name of the node to install the device in
     * \param c Container of NetDevices to add as MacSec bridge ports
     * \returns A container holding the added net device.
     */
    NetDeviceContainer Install(std::string nodeName, Ptr<NetDevice> plain_side,
                               Ptr<NetDevice> secure_side);

  private:
    ObjectFactory m_deviceFactory; //!< Object factory
    ObjectFactory m_channelFactory;
    ObjectFactory m_queueFactory;
};

} // namespace ns3

#endif /* MACSEC_TRX_HELPER_H */
