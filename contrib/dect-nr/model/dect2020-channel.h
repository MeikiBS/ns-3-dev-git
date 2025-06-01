/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DECT2020_CHANNEL_H
#define DECT2020_CHANNEL_H

#include "dect2020-phy.h"

#include "ns3/channel.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/spectrum-channel.h"

#include <vector>

namespace ns3
{

class Dect2020Phy;

enum SubslotStatus
{
    FREE = 1,
    POSSIBLE = 2,
    BUSY = 3
};

/**
 * @struct Subslot
 * @brief Represents a single subslot in the DECT-2020 NR time-frequency grid.
 *
 * A subslot is the smallest unit of time within a DECT-2020 slot and is used for precise
 * scheduling and spectrum occupancy evaluation. Each subslot can be marked as free, possibly busy,
 * or busy depending on RSSI measurements and ongoing transmissions.
 *
 * Members:
 * - `subslotId`: Index of the subslot within its parent slot.
 * - `status`: Enum representing the subslot status (e.g., free, busy).
 * - `rssi`: Measured received signal strength (RSSI) in dBm.
 * - `subslotDurationNs`: Duration of this subslot in nanoseconds.
 */
struct Subslot
{
    uint32_t subslotId;
    SubslotStatus status;
    double rssi;
    double subslotDurationNs; // Duration of a Subslot in ns
};

/**
 * @struct Slot
 * @brief Represents a DECT-2020 NR time slot, which contains multiple subslots.
 *
 * This structure is used to manage subslot scheduling, channel occupation status, and RSSI
 * tracking.
 *
 * Members:
 * - `slotId`: The index of the slot within the 10 ms frame.
 * - `subslots`: A list of subslots contained in this slot.
 */

struct Slot
{
    uint32_t slotId;
    std::vector<Subslot> subslots;
};

/**
 * \brief DECT2020 Channel Klasse
 *
 * Diese Klasse simuliert den Funkkanal zwischen DECT2020 Geräten.
 */
class Dect2020Channel : public Object
{
  public:
    static TypeId GetTypeId(void);

    Dect2020Channel();
    virtual ~Dect2020Channel();

    void AddSlot(Slot slot);

    uint32_t m_channelId; // #ETSI 103 636-2 V1.5.1, Section 5.4.2 (n)
    std::vector<Slot> m_slots;
    std::vector<Slot> m_slotsLastFrame;
    double m_centerFrequency;
};

} // namespace ns3

#endif /* DECT2020_CHANNEL_H */
