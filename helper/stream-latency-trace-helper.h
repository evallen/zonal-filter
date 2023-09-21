/**
 * \file
 * \ingroup zonal-research
 *
 * File to declare the StreamLatencyTraceHelper class.
 */
#ifndef STREAM_LATENCY_TRACING_HELPER_H
#define STREAM_LATENCY_TRACING_HELPER_H

#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"

namespace ns3
{

/**
 * \ingroup zonal-research
 *
 * Helper class to record latencies of packets in a stream from
 * one device to another. 
 *
 * This class is meant to be installed onto a sending device (CsmaNetDevice)
 * and a receiving application at another node (PacketSink). 
 *
 * It then creates a CSV at the specified location with an entry for each
 * packet that successfully travelled all the way with the fields
 *
 *  send_time,recv_time,delay
 *
 *  where
 *      send_time   - is the time that the packet was sent
 *      recv_time   - is the time that the packet was received
 *      delay       - is the difference recv_time - send_time (i.e., the latency)
 * 
 * To use it, simply call ``Install`` on the correct sending device and receiving
 * application.
 */
namespace StreamLatencyTraceHelper
{

    /**
     * Installs the trace helper onto a sending device and 
     * receiving application.
     *
     * \param sendingDevice The CsmaNetDevice to record packets leaving from.
     * \param receivingApplication The PacketSink application to record packets arriving at.
     * \param outputFilename The name of the file to write the data to.
     */
    void Install(Ptr<CsmaNetDevice> sendingDevice, 
                 Ptr<PacketSink> receivingApplication,
                 std::string outputFilename);

    /**
     * The callback that tags each packet with a send timestamp upon transmission.
     * Uses |ns3| packet tags under the hood.
     *
     * \param p The packet being sent. Tagged with a timestamp tag to mark the send time.
     */
    void SendCallback(Ptr<const Packet> p);

    /**
     * The callback that reads each incoming packet, checks its send timestamp,
     * and determines the overall latency of that packet. Writes to the specified
     * output stream.
     *
     * \param stream The output stream to write data to.
     * \param p The packet that just arrived.
     * \param address The address of the packet that just arrived.
     */
    void ReceiveCallback(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p, const Address &address);

    /**
     * Helper function to write a luine to the CSV.
     *
     * Writes the specified sendTime, recvTime, and computed delay between them.
     *
     * \param stream The output stream to write data to.
     * \param sendTime The time the packet was sent.
     * \param recvTime The time the packet was received.
     */
    void MakePacketEntry(Ptr<OutputStreamWrapper> stream, Time sendTime, Time recvTime);
};

}

#endif /* STREAM_LATENCY_TRACING_HELPER_H */
