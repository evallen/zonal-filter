/**
 * \file
 * \ingroup zonal-research
 *
 * Contains implementation of the StreamTraceHelper.
 */
#include "stream-trace-helper.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include <sstream>
#include <iomanip>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("StreamTraceHelper");

void
StreamTraceHelper::Install(Ptr<CsmaNetDevice> sendingDevice,
                                  Ptr<PacketSink> receivingApplication,
                                  std::string outputFilename)
{
    NS_LOG_FUNCTION_NOARGS();

    Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(outputFilename, std::ios::out);

    *stream->GetStream() << "send_time,recv_time,packet_size" << "\n";
    *stream->GetStream() << std::setprecision(std::numeric_limits<double>::digits10);

    sendingDevice->TraceConnectWithoutContext(
        "MacTx", 
        MakeBoundCallback(&StreamTraceHelper::SendCallback)
    );

    receivingApplication->TraceConnectWithoutContext(
        "Rx", 
        MakeBoundCallback(&StreamTraceHelper::ReceiveCallback, stream)
    );
}

void
StreamTraceHelper::SendCallback(Ptr<const Packet> p)
{
    NS_LOG_FUNCTION_NOARGS();

    TimestampTag tag;
    tag.SetTimestamp(Simulator::Now());
    p->AddPacketTag(tag);
}

void
StreamTraceHelper::ReceiveCallback(Ptr<OutputStreamWrapper> stream,
                                   Ptr<const Packet> p, const Address &address)
{
    NS_LOG_FUNCTION_NOARGS();

    TimestampTag sendTag;
    p->PeekPacketTag(sendTag);

    Time sendTime = sendTag.GetTimestamp();
    Time recvTime = Simulator::Now();
    uint16_t packetSize = p->GetSize();
    MakePacketEntry(stream, sendTime, recvTime, packetSize);
}

void
StreamTraceHelper::MakePacketEntry(Ptr<OutputStreamWrapper> stream, 
                                   Time sendTime, Time recvTime, uint16_t packetSize)
{
    NS_LOG_FUNCTION_NOARGS();

    *stream->GetStream() << sendTime.GetSeconds() << "," << recvTime.GetSeconds()
                           << "," << packetSize << "\n";
}

}
