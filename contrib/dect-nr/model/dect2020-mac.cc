/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-mac.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

#include "dect2020-net-device.h"
#include "dect2020-phy.h"

NS_LOG_COMPONENT_DEFINE ("Dect2020Mac");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Dect2020Mac);

TypeId
Dect2020Mac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Dect2020Mac")
    .SetParent<Object> ()
    .SetGroupName ("Dect2020")
    .AddConstructor<Dect2020Mac> ()
    // Hier können weitere Attribute und Trace-Quellen hinzugefügt werden
  ;
  return tid;
}

Dect2020Mac::Dect2020Mac ()
{
  NS_LOG_FUNCTION (this);
}

Dect2020Mac::~Dect2020Mac ()
{
  NS_LOG_FUNCTION (this);
}

void
Dect2020Mac::SetNetDevice (Ptr<Dect2020NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  m_device = device;
  m_address = Mac48Address::ConvertFrom (device->GetAddress ());
}

void
Dect2020Mac::SetPhy (Ptr<Dect2020Phy> phy)
{
  NS_LOG_FUNCTION (this << phy);
  m_phy = phy;
}

void
Dect2020Mac::Enqueue (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);

  // Hier können MAC-Header hinzugefügt werden
  // Für die minimale Implementierung lassen wir den Header weg

  // Senden des Pakets über die PHY-Schicht
  m_phy->Send (packet);
  
  // Trace-Aufruf
  m_txPacketTrace (packet);
}

void
Dect2020Mac::ReceiveFromPhy (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Hier können MAC-Header entfernt und überprüft werden
  // Für die minimale Implementierung gehen wir davon aus, dass kein Header vorhanden ist

  // Weiterleitung des Pakets an das NetDevice
  m_device->Receive (packet);

  // Trace-Aufruf
  m_rxPacketTrace (packet);
}

Address
Dect2020Mac::GetAddress (void) const
{
  return m_address;
}

} // namespace ns3
