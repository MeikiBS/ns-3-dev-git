/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DECT2020_PHY_H
#define DECT2020_PHY_H

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

namespace ns3
{

class Dect2020Mac;
class Dect2020Channel;
class Dect2020NetDevice;

/**
 * \brief DECT2020 PHY Klasse
 *
 * Diese Klasse implementiert die grundlegende PHY-Schichtfunktionalität
 * für das DECT2020 Modul.
 */
class Dect2020Phy : public Object
{
  public:
    static TypeId GetTypeId(void);

    Dect2020Phy();
    virtual ~Dect2020Phy();

    // Methoden zum Setzen der MAC-Schicht und des NetDevice
    void SetMac(Ptr<Dect2020Mac> mac);
    void SetNetDevice(Ptr<Dect2020NetDevice> device);

    // Methoden zum Senden und Empfangen von Paketen
    void Send(Ptr<Packet> packet);
    void Receive(Ptr<Packet> packet);

    // Kanalverwaltung
    void SetChannel(Ptr<Dect2020Channel> channel);
    Ptr<Dect2020Channel> GetChannel(void) const;

    Ptr<Dect2020NetDevice> GetNetDevice(void) const;

  private:
    void ReceiveDelayed(Ptr<Packet> packet);

    // Membervariablen
    Ptr<Dect2020Mac> m_mac;
    Ptr<Dect2020NetDevice> m_device;
    Ptr<Dect2020Channel> m_channel;

    // Sende- und Empfangsverzögerungen
    Time m_txDelay;
    Time m_rxDelay;

    // Trace-Quellen
    TracedCallback<Ptr<const Packet>> m_phyTxBeginTrace;
    TracedCallback<Ptr<const Packet>> m_phyRxEndTrace;
};

} // namespace ns3

#endif /* DECT2020_PHY_H */
