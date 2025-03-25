/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DECT2020_PHY_H
#define DECT2020_PHY_H

#include "dect2020-operating-band.h"
#include "dect2020-spectrum-signal-parameters.h"
#include "dect2020-spectrum-model-manager.h"

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/mobility-model.h"
#include "ns3/spectrum-model.h"
#include "ns3/spectrum-phy.h"

namespace ns3
{

class Dect2020Mac;
class Dect2020Channel;
class Dect2020NetDevice;
class SpectrumChannel;
struct Slot;
struct Subslot;

typedef Callback<void, Ptr<Packet>> RxReceivedCallback;

/**
 * \brief DECT2020 PHY Klasse
 *
 * Diese Klasse implementiert die grundlegende PHY-Schichtfunktionalität
 * für das DECT2020 Modul.
 */
class Dect2020Phy : public SpectrumPhy
{
  public:
    static TypeId GetTypeId(void);

    Dect2020Phy();
    virtual ~Dect2020Phy();

    // Methoden zum Setzen der MAC-Schicht und des NetDevice
    void SetMac(Ptr<Dect2020Mac> mac);
    void SetDevice(Ptr<NetDevice> device) override;

    // Methoden zum Senden und Empfangen von Paketen
    void Send(Ptr<Packet> packet);
    void Receive(Ptr<Packet> packet);

    // Kanalverwaltung
    void SetChannel(Ptr<SpectrumChannel> channel);
    Ptr<SpectrumChannel> GetChannel() const;

    Ptr<NetDevice> GetDevice() const;

    void InitializeBand(uint8_t bandNumber);
    void StartFrameTimer();
    void ProcessSlot(uint32_t slot, double slotStartTime);
    void ProcessSubslot(uint32_t slot, uint32_t subslot);
    Slot* GetCurrentSlot(uint32_t channelId) const;
    Subslot* GetCurrentSubslot(uint32_t channelId) const;
    void SetReceiveCallback(Callback<void, Ptr<Packet>> cb);
    // static const std::vector<Channel>& GetChannels();

    // Inherited Methods
    void SetMobility(Ptr<MobilityModel> m);
    Ptr<MobilityModel> GetMobility() const override;
    Ptr<const SpectrumModel> GetRxSpectrumModel() const override;
    Ptr<Object> GetAntenna() const override;
    void StartRx(Ptr<SpectrumSignalParameters> params);

  private:
    static bool m_isFrameTimerRunning;
    void ReceiveDelayed(Ptr<Packet> packet);

    // Membervariablen
    Ptr<Dect2020Mac> m_mac;
    Ptr<NetDevice> m_device;
    Ptr<SpectrumChannel> m_channel;
    uint32_t m_currentSlot;
    uint32_t m_currenSubslot;
    static std::vector<Dect2020Channel> m_channels;
    Ptr<const SpectrumModel> m_spectrumModel;
    Ptr<MobilityModel> m_mobilityModel;
    Ptr<Object> m_antenna;

    static void InitializeChannels(uint8_t bandNumber, uint8_t subcarrierScalingFactor);

    Callback<void, Ptr<Packet>> m_receiveCallback;

    // Trace-Quellen
    TracedCallback<Ptr<const Packet>> m_phyTxBeginTrace;
    TracedCallback<Ptr<const Packet>> m_phyRxTrace;
};

} // namespace ns3

#endif /* DECT2020_PHY_H */
