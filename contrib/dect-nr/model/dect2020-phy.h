/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DECT2020_PHY_H
#define DECT2020_PHY_H

#include "dect2020-physical-header-field.h"
#include "dect2020-spectrum-signal-parameters.h"

#include "ns3/mobility-model.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/spectrum-model.h"
#include "ns3/spectrum-phy.h"
#include "ns3/traced-callback.h"

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
 * \file dect2020-phy.h
 * \ingroup Dect2020Phy
 * \brief Implements the Phy layer functionality for DECT-2020 NR.
 *
 * Handles PHY procedures such Frame/Slot/Subslot Management, packet transmission and reception,
 * and interaction with the MAC layer.
 */

/**
 * \class Dect2020Phy
 * \brief DECT-2020 NR PHY layer implementation.
 *
 * Represents the Phyical Layer for a DECT-2020 NR node.
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
    void Send(Ptr<Packet> packet, Dect2020PHYControlFieldType1 physicalHeader);
    void Receive(Ptr<Packet> packet);

    // Starts the PHY Layer
    void Start();

    // Set/Get Channel
    void SetChannel(Ptr<SpectrumChannel> channel);
    Ptr<SpectrumChannel> GetChannel() const;

    // Get Device
    Ptr<NetDevice> GetDevice() const;

    // Frame/Slot/Subslot Management
    void StartFrameTimer();
    void ProcessSlot(uint32_t slot, double slotStartTime);
    void ProcessSubslot(uint32_t slotId, uint32_t subslotId);
    Slot* GetCurrentSlot(uint32_t channelId);
    Subslot* GetCurrentSubslot(uint32_t channelId);
    void SetReceiveCallback(Callback<void, Ptr<Packet>, double> cb);
    uint16_t GetCurrentAbsoluteSubslot() const;
    uint16_t GetMcsTransportBlockSize(uint8_t mu, uint8_t beta, uint8_t mcsIndex); // In bits
    double CalculateTxDurationNs(Dect2020PHYControlFieldType1 physicalHeaderField);
    Time GetAbsoluteSubslotTime(uint8_t targetSfn, uint8_t slot, uint8_t subslot) const;
    Time GetTimeUntilSubslot(uint8_t targetSfn, uint8_t targetSlot, uint8_t targetSubslot) const;
    int GetAbsoluteSubslotIndex(uint8_t slot, uint8_t subslot) const;
    Time GetTimeToNextAbsoluteSubslot(uint8_t targetSfn, uint8_t slot, uint8_t subslot) const;

    // Inherited Methods
    void SetMobility(Ptr<MobilityModel> m);
    Ptr<MobilityModel> GetMobility() const override;
    Ptr<const SpectrumModel> GetRxSpectrumModel() const override;
    Ptr<Object> GetAntenna() const override;
    void SetAntenna(Ptr<Object> antenna);
    
    void StartRx(Ptr<SpectrumSignalParameters> params);
    void EndRx(Ptr<SpectrumSignalParameters> params);

    // The currently connected channel
    Ptr<Dect2020Channel> m_dect2020Channel;

    // Current System Frame Number
    uint8_t m_currentSfn = 0;

    // The absolute subslot number in the frame
    uint16_t m_currentAbsoluteSubslot = 0;

    // The current frame start time in simulation time
    Time m_frameStartTime;

  private:
    bool m_isFrameTimerRunning = false;
    void ReceiveDelayed(Ptr<Packet> packet);

    // Membervariablen
    Ptr<Dect2020Mac> m_mac;
    Ptr<NetDevice> m_device;
    Ptr<SpectrumChannel> m_channel;
    uint32_t m_currentSlot;
    uint32_t m_currentSubslot;
    Ptr<const SpectrumModel> m_spectrumModel;
    Ptr<MobilityModel> m_mobilityModel;
    Ptr<Object> m_antenna;

    static const std::vector<uint16_t> m_singleSlotSingleStreamTransportBlockSizesMu1Beta1;

    Callback<void, Ptr<Packet>, double> m_receiveCallback;

    // Trace-Quellen
    TracedCallback<Ptr<const Packet>> m_phyTxBeginTrace;
    TracedCallback<Ptr<const Packet>> m_phyRxTrace;
};

} // namespace ns3

#endif /* DECT2020_PHY_H */
