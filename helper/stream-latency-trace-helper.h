#ifndef STREAM_LATENCY_TRACING_HELPER
#define STREAM_LATENCY_TRACING_HELPER

#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"

namespace ns3
{

namespace StreamLatencyTraceHelper
{
    void Install(Ptr<CsmaNetDevice> sendingDevice, 
                 Ptr<PacketSink> receivingApplication,
                 std::string outputFilename);

    void SendCallback(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p);
    void ReceiveCallback(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p, const Address &address);

    void MakePacketEntry(Ptr<OutputStreamWrapper> stream, Time sendTime, Time recvTime);
};

}

#endif /* STREAM_LATENCY_TRACING_HELPER */
