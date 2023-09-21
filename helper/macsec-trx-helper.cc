/**
 * Based on the BridgeHelper
 */

#include "macsec-trx-helper.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/node.h"
#include "ns3/point-to-point-module.h"
#include "ns3/bridge-module.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("MacSecTrxHelper");

MacSecTrxHelper::MacSecTrxHelper()
{
    NS_LOG_FUNCTION_NOARGS();
    m_deviceFactory.SetTypeId("ns3::PointToPointNetDevice");
    m_channelFactory.SetTypeId("ns3::PointToPointChannel");
    m_queueFactory.SetTypeId("ns3::DropTailQueue<Packet>");
}

void
MacSecTrxHelper::SetDeviceAttribute(std::string n1, const AttributeValue& v1)
{
    NS_LOG_FUNCTION_NOARGS();
    m_deviceFactory.Set(n1, v1);
}

void
MacSecTrxHelper::SetChannelAttribute(std::string n1, const AttributeValue& v1)
{
    NS_LOG_FUNCTION_NOARGS();
    m_channelFactory.Set(n1, v1);
}

void
MacSecTrxHelper::SetQueue(std::string type, StringValue maxSize)
{
    QueueBase::AppendItemTypeIfNotPresent(type, "Packet");

    m_queueFactory.SetTypeId(type);
    m_queueFactory.Set("MaxSize", maxSize);
}

NetDeviceContainer
MacSecTrxHelper::Install(Ptr<Node> node, Ptr<NetDevice> plain_side, Ptr<NetDevice> secure_side)
{
    NS_LOG_FUNCTION_NOARGS();
    NS_LOG_LOGIC("**** Install MacSec bridge device on node " << node->GetId());

    // TODO fix stupid p2p bridge implementation
    NetDeviceContainer devs;
    Ptr<PointToPointNetDevice> plain_p2p = m_deviceFactory.Create<PointToPointNetDevice>();
    Ptr<PointToPointNetDevice> secure_p2p = m_deviceFactory.Create<PointToPointNetDevice>();
    Ptr<PointToPointChannel> p2p_channel = m_channelFactory.Create<PointToPointChannel>();

    Ptr<Queue<Packet>> plain_queue = m_queueFactory.Create<Queue<Packet>>();
    Ptr<Queue<Packet>> secure_queue = m_queueFactory.Create<Queue<Packet>>();

    plain_p2p->SetQueue(plain_queue);
    secure_p2p->SetQueue(secure_queue);

    plain_p2p->Attach(p2p_channel);
    secure_p2p->Attach(p2p_channel);

    devs.Add(plain_p2p);
    devs.Add(secure_p2p);

    node->AddDevice(plain_p2p);
    node->AddDevice(secure_p2p);

    NetDeviceContainer plain_devs;
    plain_devs.Add(plain_side);
    plain_devs.Add(plain_p2p);

    NetDeviceContainer secure_devs;
    secure_devs.Add(secure_side);
    secure_devs.Add(secure_p2p);

    BridgeHelper bridge;
    bridge.Install(node, plain_devs);
    bridge.Install(node, secure_devs);

    // NetDeviceContainer devs;
    // Ptr<ZonalControllerNetDevice> dev = m_deviceFactory.Create<ZonalControllerNetDevice>();
    // devs.Add(dev);
    // node->AddDevice(dev);
    //
    // for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    // {
    //     NS_LOG_LOGIC("**** Add Zonal Bridge Port " << *i);
    //     dev->AddBridgePort(*i);
    // }
    //
    return devs;
}

NetDeviceContainer
MacSecTrxHelper::Install(std::string nodeName, Ptr<NetDevice> plain_side, Ptr<NetDevice> secure_side)
{
    NS_LOG_FUNCTION_NOARGS();
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return Install(node, plain_side, secure_side);
}

} // namespace ns3
