/**
 * \file
 * \ingroup zonal-research
 *
 * CLass ProcessingBridgeNetDevice declaration, along with
 * helper class QueuedPacket declaration & implementation.
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
 * \ingroup zonal-research
 *
 * Helper class to represent a packet stored in an input queue
 * after being received on an interface.
 */
class QueuedPacket : public Object {
public:
    /** The NetDevice the packet entered on. */
    Ptr<NetDevice> device;

    /** The packet itself. */
    Ptr<const Packet> packet;

    /** The protocol ID of the packet. */
    uint16_t protocol;

    /** The source address that sent the packet. */
    const Address source;

    /** The destination address of the packet. */
    const Address destination;

    /** The type of the packet. */
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
 * \ingroup zonal-research
 *
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

        /**
         * Override from BridgeNetDevice::ReceiveFromDevice.
         *
         * Wrapper function that 
         *  (1) makes sure the necessary queues are intialized,
         *  (2) enqueues the incoming packet for processing, and
         *  (3) if the processing state machine is ready, processes that packet.
         *
         * (From BridgeNetDevice::ReceiveFromDevice:)
         * \param device the originating port
         * \param packet the received packet
         * \param protocol the packet protocol (e.g., Ethertype)
         * \param source the packet source
         * \param destination the packet destination
         * \param packetType the packet type (e.g., host, broadcast, etc.)
         */
        void ReceiveFromDevice(Ptr<NetDevice> device,
                               Ptr<const Packet> packet,
                               uint16_t protocol,
                               const Address& source,
                               const Address& destination,
                               PacketType packetType) override;

        /**
         * Computes the latency of a packet given its size in 
         * bytes. We define this as a callback because we might 
         * want to simulate it as a distribution or have it depend
         * on the size in bytes of the packet.
         * 
         * \param size The size of the packet in bytes.
         *
         * \return The time it takes to process that packet.
         */
        typedef
        Callback<Time, uint32_t> LatencyCallback;
    
    private:
        /** Determines the latency of a given packet. */
        LatencyCallback m_processingDelayCalculator;
        
        /** How fast this bridge processes packets. */
        DataRate m_throughput;
        
        /** 
         * The size of the input queue on each input port
         * of the bridge. Right now, a unified queue is not
         * implemented yet (but could be in the future!)
         */
        QueueSize m_queueSize;

        /** Input queues and associated states. */
        enum ProcessingState {
            READY, //!< The processing machine is ready to process the next packet.
            BUSY //!< The processing machine is not ready to process the next packet.
        };

        /** Map of device IDs to processing states. */
        std::unordered_map<uint32_t, ProcessingState> m_queueStates;

        /** Map of device IDs to input queues. */
        std::unordered_map<uint32_t, Ptr<Queue<QueuedPacket>>> m_inputQueues;


        /**
         * Function to mark a packet as beginning processing and to prepare
         * it to continue transmission after a delay.
         *
         * (From BridgeNetDevice::ReceiveFromDevice:)
         * \param device the originating port
         * \param packet the received packet
         * \param protocol the packet protocol (e.g., Ethertype)
         * \param source the packet source
         * \param destination the packet destination
         * \param packetType the packet type (e.g., host, broadcast, etc.)
         */
        void StartProcessingPacket(Ptr<NetDevice> device,
                                   Ptr<const Packet> packet,
                                   uint16_t protocol,
                                   const Address& source,
                                   const Address& destination,
                                   PacketType packetType);

        /**
         * Wrapper function around BridgeNetDevice::ReceiveFromDevice
         * because we can't call that one as a callback since it's protected - 
         * but we can call this as a callback, which itself can directly call
         * BridgeNetDevice::ReceiveFromDevice. Probably easier just to look
         * at the implementation if you're confused.
         *
         * (From BridgeNetDevice::ReceiveFromDevice:)
         * \param device the originating port
         * \param packet the received packet
         * \param protocol the packet protocol (e.g., Ethertype)
         * \param source the packet source
         * \param destination the packet destination
         * \param packetType the packet type (e.g., host, broadcast, etc.)
         */
        void SuperReceiveFromDevice(Ptr<NetDevice> device,
                                    Ptr<const Packet> packet,
                                    uint16_t protocol,
                                    const Address& source,
                                    const Address& destination,
                                    PacketType packetType);

        /**
         * Callback for when a processing machine finishes processing
         * a packet from a given input port and is ready to process
         * the next one.
         *
         * \param interface The interface ID to mark as ready for the 
         *                  next packet (see the map ``m_queueStates``).
         */
        void ReadyToProcessNextPacket(uint32_t interface);

        /**
         * Helper function to determine the 'transmission' time for a given
         * packet to pass through the processing bridge. 
         *
         * In other words, this is how long you must wait until the next packet
         * can begin processing. Note that this is different from latency or
         * 'processing time' - another packet can begin processing while this one
         * hasn't sent yet if the device implements some sort of pipeline.
         *
         * Based on the size of the packet and the throughput
         * of the processing bridge.
         *
         * \param packet_size The size of the packet to process in bytes.
         * 
         * \return The time until the next packet can begin processing.
         */
        Time GetThroughputDelay(uint32_t packet_size);
};


}

#endif /* PROCESSING_BRIDGE_NET_DEVICE_H */
