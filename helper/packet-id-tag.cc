#include "packet-id-tag.h"

#include "ns3/tag-buffer.h"
#include "ns3/tag.h"
#include "ns3/type-id.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(PacketIDTag);

PacketIDTag::PacketIDTag() = default;

PacketIDTag::PacketIDTag(uint64_t id)
    : m_id(id)
{
}

TypeId
PacketIDTag::GetTypeId()
{
    static TypeId tid = TypeId("ns3::PacketIDTag")
                            .SetParent<Tag>()
                            .SetGroupName("zonal-research")
                            .AddConstructor<PacketIDTag>();
    return tid;
}

TypeId
PacketIDTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
PacketIDTag::GetSerializedSize() const
{
    return sizeof(uint64_t);
}

void
PacketIDTag::Serialize(TagBuffer i) const
{
    i.WriteU64(m_id);
}

void
PacketIDTag::Deserialize(TagBuffer i)
{
    m_id = i.ReadU64();
}

void
PacketIDTag::Print(std::ostream& os) const
{
    os << "id=" << m_id;
}

uint64_t
PacketIDTag::GetID() const
{
    return m_id;
}

void
PacketIDTag::SetID(uint64_t id)
{
    m_id = id;
}

} // namespace ns3
