/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DECT2020_CHANNEL_H
#define DECT2020_CHANNEL_H

#include "dect2020-phy.h"

#include "ns3/channel.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"

#include <vector>

namespace ns3
{

class Dect2020Phy;

/**
 * \brief DECT2020 Channel Klasse
 *
 * Diese Klasse simuliert den Funkkanal zwischen DECT2020 Geräten.
 */
class Dect2020Channel : public Channel
{
  public:
    static TypeId GetTypeId(void);

    Dect2020Channel();
    virtual ~Dect2020Channel();

    // Hinzufügen von PHYs zum Kanal
    void AddPhy(Ptr<Dect2020Phy> phy);

    // Übertragung von Paketen
    void Transmit(Ptr<Packet> packet, Ptr<Dect2020Phy> sender);

    // Implementierung der virtuellen Methoden von ns3::Channel
    virtual std::size_t GetNDevices(void) const override;
    virtual Ptr<NetDevice> GetDevice(std::size_t i) const override;

  private:
    // Liste der angeschlossenen PHYs
    std::vector<Ptr<Dect2020Phy>> m_phyList;

    // Signalverzögerung
    Time m_delay;
};

} // namespace ns3

#endif /* DECT2020_CHANNEL_H */
