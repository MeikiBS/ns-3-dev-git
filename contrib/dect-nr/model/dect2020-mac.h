#ifndef DECT2020_MAC_H
#define DECT2020_MAC_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"

#include "ns3/packet.h"

namespace ns3 {

class Dect2020NetDevice;
class Dect2020Phy;

/**
 * \brief DECT2020 MAC Klasse
 *
 * Diese Klasse implementiert die grundlegende MAC-Schichtfunktionalität
 * für das DECT2020 Modul.
 */
class Dect2020Mac : public Object
{
public:
  static TypeId GetTypeId (void);

  Dect2020Mac ();
  virtual ~Dect2020Mac ();

  // Methoden zum Setzen der NetDevice und Phy
  void SetNetDevice (Ptr<Dect2020NetDevice> device);
  void SetPhy (Ptr<Dect2020Phy> phy);

  // Methoden zum Enqueue und Dequeue von Paketen
  void Enqueue (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

  // Empfang von Paketen von der PHY-Schicht
  void ReceiveFromPhy (Ptr<Packet> packet);

  // Zugriff auf die Adresse des NetDevice
  Address GetAddress (void) const;

private:
  // Membervariablen
  Ptr<Dect2020NetDevice> m_device;
  Ptr<Dect2020Phy> m_phy;
  Mac48Address m_address;

  // Sendewarteschlange (für zukünftige Erweiterungen)
  // std::queue<Ptr<Packet>> m_txQueue;

  // Trace-Quellen
  TracedCallback<Ptr<const Packet>> m_txPacketTrace;
  TracedCallback<Ptr<const Packet>> m_rxPacketTrace;
};

} // namespace ns3

#endif /* DECT2020_MAC_H */
