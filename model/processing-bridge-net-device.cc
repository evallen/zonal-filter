
#include "processing-bridge-net-device.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("ProcessingBridgeNetDevice");

NS_OBJECT_ENSURE_REGISTERED(ProcessingBridgeNetDevice);
NS_OBJECT_TEMPLATE_CLASS_DEFINE(Queue, QueuedPacket);
NS_OBJECT_TEMPLATE_CLASS_DEFINE(DropTailQueue, QueuedPacket);

TypeId
ProcessingBridgeNetDevice::GetTypeId()
{
    static TypeId tid = 
        TypeId("ns3::ProcessingBridgeNetDevice")
            .SetParent<BridgeNetDevice>()
            .SetGroupName("Bridge")
            .AddConstructor<ProcessingBridgeNetDevice>()
            .AddAttribute("ProcessingDelay",
                          "The time it takes for the packet to be processed, "
                          "including any table / security lookups.",
                          CallbackValue(LatencyCallback([] (uint32_t size) {return Time("30ns");})),
                          MakeCallbackAccessor(&ProcessingBridgeNetDevice::m_processingDelayCalculator),
                          MakeCallbackChecker())
            .AddAttribute("Throughput",
                          "The data rate at which the device processes packets from "
                          "incoming net devices.",
                          DataRateValue(DataRate("1000Mbps")),
                          MakeDataRateAccessor(&ProcessingBridgeNetDevice::m_throughput),
                          MakeDataRateChecker())
            .AddAttribute("QueueSize",
                          "The size of each input queue (please see the individual "
                          "linked network devices for output queue sizes).",
                          QueueSizeValue(QueueSize("100p")),
                          MakeQueueSizeAccessor(&ProcessingBridgeNetDevice::m_queueSize),
                          MakeQueueSizeChecker());

    return tid;
}

ProcessingBridgeNetDevice::ProcessingBridgeNetDevice()
{
    NS_LOG_FUNCTION_NOARGS();
}

ProcessingBridgeNetDevice::~ProcessingBridgeNetDevice()
{
    NS_LOG_FUNCTION_NOARGS();
}

void
ProcessingBridgeNetDevice::ReceiveFromDevice(Ptr<NetDevice> incomingPort,
                                            Ptr<const Packet> packet,
                                            uint16_t protocol,
                                            const Address& src,
                                            const Address& dst,
                                            PacketType packetType)
{
    NS_LOG_FUNCTION_NOARGS();


    uint32_t interface = incomingPort->GetIfIndex();

    // Initialize default values if necessary
    if (!m_queueStates.count(interface)) {
        m_queueStates[interface] = READY;
    }
    if (!m_inputQueues.count(interface)) {
        m_inputQueues[interface] = CreateObject<DropTailQueue<QueuedPacket>>();
        m_inputQueues[interface]->SetMaxSize(m_queueSize);
    }
    Ptr<Queue<QueuedPacket>> queue = m_inputQueues[interface];

    queue->Enqueue(Create<QueuedPacket>(incomingPort, packet, protocol, src, dst, packetType));
    
    // Start sending immediately if nothing is in the queue
    if (m_queueStates[interface] == READY) {
        Ptr<QueuedPacket> next = queue->Dequeue();
        StartProcessingPacket(next->device, next->packet, next->protocol, next->source,
                              next->destination, next->packetType);
    }
}

void
ProcessingBridgeNetDevice::SuperReceiveFromDevice(Ptr<NetDevice> incomingPort,
                                                  Ptr<const Packet> packet,
                                                  uint16_t protocol,
                                                  const Address& src,
                                                  const Address& dst,
                                                  PacketType packetType)
{
    BridgeNetDevice::ReceiveFromDevice(incomingPort, packet, protocol, src, dst, packetType);
}

void
ProcessingBridgeNetDevice::StartProcessingPacket(Ptr<NetDevice> incomingPort,
                                                 Ptr<const Packet> packet,
                                                 uint16_t protocol,
                                                 const Address& src,
                                                 const Address& dst,
                                                 PacketType packetType)
{
    NS_LOG_FUNCTION_NOARGS();

    uint32_t interface = incomingPort->GetIfIndex();
    NS_ASSERT_MSG(m_queueStates[interface] == READY, "Processing state of interface must be READY");

    m_queueStates[interface] = BUSY;

    BridgeNetDevice::ReceiveFromDevice(incomingPort, packet, protocol, src, dst, packetType);

    // Schedule when packet leaves to simulate latency.
    Simulator::Schedule(m_processingDelayCalculator(packet->GetSize()),
                        &ProcessingBridgeNetDevice::SuperReceiveFromDevice,
                        this,
                        incomingPort,
                        packet,
                        protocol,
                        src,
                        dst,
                        packetType);

    // Schedule when next packet can begin processing to simulate throughput.
    Simulator::Schedule(GetThroughputDelay(packet->GetSize()), 
                         &ProcessingBridgeNetDevice::ReadyToProcessNextPacket,
                         this,
                         interface);
}

void
ProcessingBridgeNetDevice::ReadyToProcessNextPacket(uint32_t interface)
{
    NS_LOG_FUNCTION_NOARGS();

    m_queueStates[interface] = READY;

    // Transmit next if there is more
    Ptr<Queue<QueuedPacket>> queue = m_inputQueues[interface];
    if (!queue->IsEmpty()) {
        Ptr<QueuedPacket> next = queue->Dequeue();
        StartProcessingPacket(next->device, next->packet, next->protocol, 
                              next->source, next->destination, next->packetType);
    }
}

Time
ProcessingBridgeNetDevice::GetThroughputDelay(uint32_t packet_size)
{
    NS_LOG_FUNCTION_NOARGS();

    return m_throughput.CalculateBytesTxTime(packet_size);
}

}
