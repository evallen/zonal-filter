#ifndef PACKET_ID_TAG_H
#define PACKET_ID_TAG_H

#include "ns3/tag-buffer.h"
#include "ns3/tag.h"
#include "ns3/type-id.h"

#include <iostream>

namespace ns3
{

/**
 * Packet ID tag for associating an ID number with a packet.
 */
class PacketIDTag : public Tag
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    /**
     * \brief Construct a new PacketIDTag object
     */
    PacketIDTag();

    /**
     * \brief Construct a new PacketIDTag object with the given id
     * \param id The id
     */
    PacketIDTag(uint64_t id);

    void Serialize(TagBuffer i) const override;
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;

    /**
     * \brief Get the ID
     * \return Time for this tag
     */
    uint64_t GetID() const;

    /**
     * \brief Set the ID
     * \param id ID to assign to tag
     */
    void SetID(uint64_t id);

  private:
    uint64_t m_id{0};
};

} // namespace ns3

#endif // PACKET_ID_TAG_H
