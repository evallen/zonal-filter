/**
 * \file
 * \ingroup zonal-research
 *
 * Contains implementation of the StreamTraceHelper.
 */
#include "stream-trace-helper.h"
#include "packet-id-tag.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include <sstream>
#include <iomanip>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("StreamTraceHelper");

StreamTraceHelper::StreamTraceHelper(Ptr<OnOffApplication> sendingApplication,
                                     Ptr<PacketSink> receivingApplication,
                                     std::string outputFilename,
                                     Time startTime)
    : m_outputFilename(outputFilename),
      m_state(WAITING)
{
    NS_LOG_FUNCTION(this << sendingApplication << receivingApplication << outputFilename);

    Ptr<OutputStreamWrapper> stream = 
        Create<OutputStreamWrapper>(outputFilename + ".csv", std::ios::out);

    *stream->GetStream() << "send_time,recv_time,packet_size" << "\n";
    *stream->GetStream() << std::setprecision(std::numeric_limits<double>::digits10);

    sendingApplication->TraceConnectWithoutContext(
        "BeforeTx", 
        MakeCallback(&StreamTraceHelper::SendCallback, this)
    );

    receivingApplication->TraceConnectWithoutContext(
        "Rx", 
        MakeCallback(&StreamTraceHelper::ReceiveCallback, this, stream)
    );

    Simulator::Schedule(startTime,
                        &StreamTraceHelper::Activate,
                        this);
}

void
StreamTraceHelper::SendCallback(Ptr<const Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    TimestampTag tsTag;
    tsTag.SetTimestamp(Simulator::Now());
    p->AddPacketTag(tsTag);

    PacketIDTag idTag;
    idTag.SetID(m_rawSentPackets);
    p->AddPacketTag(idTag);

    m_rawSentPackets++;

    if (m_state == ON)
    {
        m_sentPackets++;
    }
}

void
StreamTraceHelper::ReceiveCallback(Ptr<OutputStreamWrapper> stream,
                                   Ptr<const Packet> p, const Address &address)
{
    NS_LOG_FUNCTION(this << stream << p << address);

    TimestampTag tsTag;
    p->PeekPacketTag(tsTag);

    PacketIDTag idTag;
    p->PeekPacketTag(idTag);

    NS_LOG_DEBUG("RECEIVED PACKET " << idTag.GetID());
    if (m_state == ON && idTag.GetID() >= m_firstPacketToTrack)
    {
        Time sendTime = tsTag.GetTimestamp();
        Time recvTime = Simulator::Now();
        uint16_t packetSize = p->GetSize();
        MakePacketEntry(stream, sendTime, recvTime, packetSize);

        m_receivedPackets++;
    }
}

void
StreamTraceHelper::MakePacketEntry(Ptr<OutputStreamWrapper> stream, 
                                   Time sendTime, Time recvTime, uint16_t packetSize)
{
    NS_LOG_FUNCTION(this << stream << sendTime << recvTime << packetSize);

    *stream->GetStream() << sendTime.GetSeconds() << "," << recvTime.GetSeconds()
                           << "," << packetSize << "\n";
}

void
StreamTraceHelper::LogFinalMetrics()
{
    NS_LOG_FUNCTION(this);

    Ptr<OutputStreamWrapper> finalStream = 
        Create<OutputStreamWrapper>(m_outputFilename + ".json", std::ios::out);

    NS_LOG_DEBUG("m_sentPackets: " << m_sentPackets);
    NS_LOG_DEBUG("m_receivedPackets: " << m_receivedPackets);
    double dropRate = 1 - ((double)m_receivedPackets / (double)m_sentPackets);

    *finalStream->GetStream() << "{" << std::endl;
    *finalStream->GetStream() << "\t\"dropRate\": " << dropRate << std::endl;
    *finalStream->GetStream() << "}" << std::endl;
}

void
StreamTraceHelper::Activate()
{
    NS_LOG_FUNCTION(this);

    m_state = ON;
    m_firstPacketToTrack = m_rawSentPackets;

    NS_LOG_DEBUG("Activating StreamTraceHelper " << this << 
                 " at packet ID " << m_firstPacketToTrack);
}

}
