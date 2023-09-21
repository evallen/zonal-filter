/**
 * \file
 * \ingroup zonal-research
 *
 * Based on the BridgeHelper.
 * Contains the implementation of the ProcessingBridgeHelper.
 */

#include "processing-bridge-helper.h"

#include "ns3/processing-bridge-net-device.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/node.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("ProcessingBridgeHelper");

ProcessingBridgeHelper::ProcessingBridgeHelper()
{
    NS_LOG_FUNCTION_NOARGS();
    m_deviceFactory.SetTypeId("ns3::ProcessingBridgeNetDevice");
}

void
ProcessingBridgeHelper::SetDeviceAttribute(std::string n1, const AttributeValue& v1)
{
    NS_LOG_FUNCTION_NOARGS();
    m_deviceFactory.Set(n1, v1);
}

NetDeviceContainer
ProcessingBridgeHelper::Install(Ptr<Node> node, NetDeviceContainer c)
{
    NS_LOG_FUNCTION_NOARGS();
    NS_LOG_LOGIC("**** Install zonal bridge device on node " << node->GetId());

    NetDeviceContainer devs;
    Ptr<ProcessingBridgeNetDevice> dev = m_deviceFactory.Create<ProcessingBridgeNetDevice>();
    devs.Add(dev);
    node->AddDevice(dev);

    for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        NS_LOG_LOGIC("**** Add Zonal Bridge Port " << *i);
        dev->AddBridgePort(*i);
    }
    return devs;
}

NetDeviceContainer
ProcessingBridgeHelper::Install(std::string nodeName, NetDeviceContainer c)
{
    NS_LOG_FUNCTION_NOARGS();
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return Install(node, c);
}

} // namespace ns3
