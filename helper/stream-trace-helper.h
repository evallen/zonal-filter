/**
 * \file
 * \ingroup zonal-research
 *
 * File to declare the StreamTraceHelper class.
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
 *  send_time,recv_time,packet_size
 *
 *  where
 *      send_time       - is the time that the packet was sent
 *      recv_time       - is the time that the packet was received
 *      packet_size     - is the size of the IP payload of the packet
 * 
 * To use it, simply construct an instance on the correct sending device and receiving
 * application.
 */
class StreamTraceHelper
{

public:
    /**
     * Installs the trace helper onto a sending device and 
     * receiving application.
     *
     * \param sendingApplication The CsmaNetDevice to record packets leaving from.
     * \param receivingApplication The PacketSink application to record packets arriving at.
     * \param outputFilename The name of the file to write the data to (no extension).
     * \param startTime The time to start tracing.
     */
    StreamTraceHelper(Ptr<OnOffApplication> sendingApplication, 
                      Ptr<PacketSink> receivingApplication,
                      std::string outputFilename,
                      Time startTime);

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
     * Log final metrics such as packet drop rate to a .json file.
     * Should be called at the end of measurement.
     */
    void LogFinalMetrics();

    enum State
    {
        WAITING,
        ON
    };

private:
    uint64_t m_rawSentPackets = 0; //!< Actual number of packets sent, even before we begin tracking.
    uint64_t m_sentPackets = 0; //!< Number of packets sent while tracking.
    uint64_t m_receivedPackets = 0; //!< Number of packets received while tracking. 
    std::string m_outputFilename; //!< Where to output the result files.

    State m_state; //!< State machine for the class. Controls whether it is on or off.
    uint64_t m_firstPacketToTrack = 0; //!< The ID of the first packet to start tracking.

    /**
     * Helper function to write a line to the CSV.
     *
     * Writes the specified sendTime, recvTime, and computed delay between them.
     *
     * \param stream The output stream to write data to.
     * \param sendTime The time the packet was sent.
     * \param recvTime The time the packet was received.
     */
    void MakePacketEntry(Ptr<OutputStreamWrapper> stream, Time sendTime, Time recvTime,
                         uint16_t packetSize);

    /**
     * Turn on stream tracing. Useful to start it at a certain time.
     */
    void Activate();
};

}

#endif /* STREAM_LATENCY_TRACING_HELPER_H */
