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

struct Subslot
{
  uint32_t subslotId;
  SubslotStatus status;
  double rssi;
  double subslotDurationNs; // Duration of a Subslot in ns
};

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

    uint32_t m_channelId;   // #ETSI 103 636-2 V1.5.1, Section 5.4.2 (n)
    uint8_t m_subcarrierScalingFactor;
    std::vector<Slot> m_slots;
    std::vector<Slot> m_slotsLastFrame;
    double m_centerFrequency;
};

} // namespace ns3

#endif /* DECT2020_CHANNEL_H */
