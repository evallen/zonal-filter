/**
 * \file
 * \ingroup zonal-research
 *
 * Contains implementation of the StreamLatencyTraceHelper.
 */
#include "stream-latency-trace-helper.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include <sstream>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("StreamLatencyTraceHelper");

void
StreamLatencyTraceHelper::Install(Ptr<CsmaNetDevice> sendingDevice,
                                  Ptr<PacketSink> receivingApplication,
                                  std::string outputFilename)
{
    NS_LOG_FUNCTION_NOARGS();

    Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(outputFilename, std::ios::out);

    *stream->GetStream() << "send_time,recv_time,delay" << "\n";

    sendingDevice->TraceConnectWithoutContext(
        "MacTx", 
        MakeBoundCallback(&StreamLatencyTraceHelper::SendCallback)
    );

    receivingApplication->TraceConnectWithoutContext(
        "Rx", 
        MakeBoundCallback(&StreamLatencyTraceHelper::ReceiveCallback, stream)
    );
}

void
StreamLatencyTraceHelper::SendCallback(Ptr<const Packet> p)
{
    NS_LOG_FUNCTION_NOARGS();

    TimestampTag tag;
    tag.SetTimestamp(Simulator::Now());
    p->AddPacketTag(tag);
}

void
StreamLatencyTraceHelper::ReceiveCallback(Ptr<OutputStreamWrapper> stream,
                                          Ptr<const Packet> p, const Address &address)
{
    NS_LOG_FUNCTION_NOARGS();

    TimestampTag sendTag;
    p->PeekPacketTag(sendTag);

    Time sendTime = sendTag.GetTimestamp();
    Time recvTime = Simulator::Now();
    MakePacketEntry(stream, sendTime, recvTime);
}

void
StreamLatencyTraceHelper::MakePacketEntry(Ptr<OutputStreamWrapper> stream, 
                                          Time sendTime, Time recvTime)
{
    NS_LOG_FUNCTION_NOARGS();

    Time delayTime = recvTime - sendTime;

    *stream->GetStream() << sendTime.GetSeconds() << "," << recvTime.GetSeconds()
                           << "," << delayTime.GetSeconds() << "\n";
}

}
