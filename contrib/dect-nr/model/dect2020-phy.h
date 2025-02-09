/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DECT2020_PHY_H
#define DECT2020_PHY_H

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "dect2020-operating-band.h"

namespace ns3
{

class Dect2020Mac;
class Dect2020Channel;
class Dect2020NetDevice;
class SpectrumChannel;
struct Slot;
struct Subslot;

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
    void SetChannel(Ptr<SpectrumChannel> channel);
    Ptr<SpectrumChannel> GetChannel(void) const;

    Ptr<Dect2020NetDevice> GetNetDevice(void) const;

    void InitializeBand(uint8_t bandNumber);
    void StartFrameTimer();
    void ProcessSlot(uint32_t slot, uint64_t slotStartTime);
    void ProcessSubslot(uint32_t slot, uint32_t subslot);
    Slot* GetCurrentSlot(uint32_t channelId) const;
    Subslot* GetCurrentSubslot(uint32_t channelId) const;

    // static const std::vector<Channel>& GetChannels();

  private:
    void ReceiveDelayed(Ptr<Packet> packet);

    // Membervariablen
    Ptr<Dect2020Mac> m_mac;
    Ptr<Dect2020NetDevice> m_device;
    Ptr<SpectrumChannel> m_channel;
    uint32_t m_currentSlot;
    uint32_t m_currenSubslot;
    static std::vector<Dect2020Channel> m_channels;
    
    static void InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor);

    // Trace-Quellen
    TracedCallback<Ptr<const Packet>> m_phyTxBeginTrace;
    TracedCallback<Ptr<const Packet>> m_phyRxEndTrace;
};

} // namespace ns3

#endif /* DECT2020_PHY_H */
