/**
 * Bridge net device that includes processing delay
 * and input queues with a predefined throughput
 */

#ifndef PROCESSING_BRIDGE_NET_DEVICE_H
#define PROCESSING_BRIDGE_NET_DEVICE_H

#include "ns3/core-module.h"
#include "ns3/bridge-module.h"
#include "ns3/network-module.h"

#include <map>

namespace ns3
{

/**
 * Helper class to represent a packet stored in an input queue
 * after being received on an interface.
 */
class QueuedPacket : public Object {
public:
    Ptr<NetDevice> device;
    Ptr<const Packet> packet;
    uint16_t protocol;
    const Address& source;
    const Address& destination;
    NetDevice::PacketType packetType;

    QueuedPacket(Ptr<NetDevice> device, Ptr<const Packet> packet,
                 uint16_t protocol, const Address& source, 
                 const Address& destination, NetDevice::PacketType packetType) :
        device(device),
        packet(packet),
        protocol(protocol),
        source(source),
        destination(destination),
        packetType(packetType) {};

    QueuedPacket();

    uint32_t GetSize() const { return packet->GetSize(); }

    static TypeId GetTypeId() 
    { 
        static TypeId tid = TypeId("ns3::QueuedPacket")
            .SetParent<Object>()
            .AddConstructor<QueuedPacket>();
        return tid;
    }
};

/**
 * NetDevice that acts like a BridgeNetDevice but also
 * simulates the time it takes to process each packet.
 *
 * Provides the ability to specify processing delays for each
 * packet, an overall throughput, and an input queue so you
 * can simulate dropping packets on input and more.
 */
class ProcessingBridgeNetDevice : public BridgeNetDevice 
{
    public:
        
        static TypeId GetTypeId();
        ProcessingBridgeNetDevice();
        ~ProcessingBridgeNetDevice() override;

        // Override
        void ReceiveFromDevice(Ptr<NetDevice> device,
                               Ptr<const Packet> packet,
                               uint16_t protocol,
                               const Address& source,
                               const Address& destination,
                               PacketType packetType);

        // Computes the latency of a packet given its size in 
        // bytes. We define this as a callback because we might 
        // want to simulate it as a distribution or have it depend
        // on the size in bytes of the packet.
        //
        // Parameters:
        //   uint32_t size  - The size of the packet in bytes.
        //
        // Returns:
        //   Time time      - The time it takes to process the packet.
        typedef
        Callback<Time, uint32_t> LatencyCallback;
    
    private:
        // Attributes
        LatencyCallback m_processingDelayCalculator;
        DataRate m_throughput;
        QueueSize m_queueSize;

        // Input queues and associated states
        enum ProcessingState {
            READY,
            BUSY
        };
        std::unordered_map<uint32_t, ProcessingState> m_queueStates;
        std::unordered_map<uint32_t, Ptr<Queue<QueuedPacket>>> m_inputQueues;


        void StartProcessingPacket(Ptr<NetDevice> device,
                                   Ptr<const Packet> packet,
                                   uint16_t protocol,
                                   const Address& source,
                                   const Address& destination,
                                   PacketType packetType);

        // Wrapper function around BridgeNetDevice::ReceiveFromDevice
        // because we can't call that one as a callback since it's protected - 
        // but we can call this as a callback, which itself can directly call
        // BridgeNetDevice::ReceiveFromDevice. Probably easier just to look
        // at the implementation if you're confused.
        void SuperReceiveFromDevice(Ptr<NetDevice> device,
                                    Ptr<const Packet> packet,
                                    uint16_t protocol,
                                    const Address& source,
                                    const Address& destination,
                                    PacketType packetType);

        void ReadyToProcessNextPacket(uint32_t interface);

        Time GetThroughputDelay(uint32_t packet_size);
};


}

#endif /* PROCESSING_BRIDGE_NET_DEVICE_H */
