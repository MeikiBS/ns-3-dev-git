#ifndef DECT2020_NET_DEVICE_H
#define DECT2020_NET_DEVICE_H

#include "ns3/callback.h"
#include "ns3/mac48-address.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

#include <iomanip> // Für std::setw und std::setfill

namespace ns3
{

class Dect2020Mac;
class Dect2020Phy;

/**
 * \brief DECT2020 NetDevice Klasse
 *
 * Diese Klasse repräsentiert ein DECT2020 Netzwerkgerät in ns-3.
 */
class Dect2020NetDevice : public NetDevice
{
  public:
    /**
     * The possible Termination Point Types (PT or FT)
     */
    enum TerminationPointType
    {
        PT, // Portable Termination
        FT  // Fixed Termination
    };

    static TypeId GetTypeId(void);

    Dect2020NetDevice();
    virtual ~Dect2020NetDevice();

    // Spezielle DECT 2020 Methoden

    /**
     * \brief Sets the termination point type of the device.
     *
     * Note: In typical DECT implementations, the termination point type
     * (PT or FT) would be dynamically determined by the device. However,
     * for simplicity and initial testing purposes, the type is set manually
     * during device creation in this simulation.
     */
    void SetTerminationPointType(TerminationPointType tpm);
    /**
     * \brief Gets the termination point type of the device.
     * \return The termination point type of the device (PT or FT)
     */
    TerminationPointType GetTerminationPointType();

    // Vererbte Methoden von NetDevice
    virtual void SetIfIndex(const uint32_t index) override;
    virtual uint32_t GetIfIndex(void) const override;

    virtual Ptr<Channel> GetChannel(void) const override;

    virtual void SetAddress(Address address) override;
    virtual Address GetAddress(void) const override;

    virtual bool SetMtu(const uint16_t mtu) override;
    virtual uint16_t GetMtu(void) const override;

    virtual bool IsLinkUp(void) const override;
    virtual void AddLinkChangeCallback(Callback<void> callback) override;

    virtual bool IsBroadcast(void) const override;
    virtual Address GetBroadcast(void) const override;

    virtual bool IsMulticast(void) const override;
    virtual Address GetMulticast(Ipv4Address multicastGroup) const override;
    virtual Address GetMulticast(Ipv6Address multicastGroup) const override;

    virtual bool IsPointToPoint(void) const override;

    virtual bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;
    virtual bool SendFrom(Ptr<Packet> packet,
                          const Address& source,
                          const Address& dest,
                          uint16_t protocolNumber) override;

    virtual Ptr<Node> GetNode(void) const override;
    virtual void SetNode(Ptr<Node> node) override;

    virtual bool NeedsArp(void) const override;

    virtual void SetReceiveCallback(NetDevice::ReceiveCallback cb) override;
    virtual void SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb) override;

    virtual bool SupportsSendFrom(void) const override;

    virtual bool IsBridge(void) const override;

    // DECT2020-spezifische Methoden
    void SetMac(Ptr<Dect2020Mac> mac);
    Ptr<Dect2020Mac> GetMac(void) const;

    void SetPhy(Ptr<Dect2020Phy> phy);
    Ptr<Dect2020Phy> GetPhy(void) const;

    void Receive(Ptr<Packet> packet);

    void SetLinkUp(void);
    void SetLinkDown(void);

    void SetBandNumber(uint8_t bandNumber);
    uint8_t GetBandNumber(void) const;

    TerminationPointType m_terminationPointType;

    void SetBootstrappingVariables(bool isFT);

    // --- Bootstrapping variables ---

    // RD Capability Values --> ETSI TS 103636 044 V 2.1.1 Table 6.4.3.5-1
    uint8_t m_numOfPHYCapabilities;
    uint8_t m_release; // Indicates release of the radio device. Coded values: reserved, Release 1,
                       // Release 2, Release 3, Release 4, rest of the values are reserved
    bool m_supportGroupAssignment ; // Indicates if the device supports group assignment
    bool m_supportPaging;       // Indicates if the device supports paging
    uint8_t m_operatingModes;      // Indicates the operating modes of the device
    bool m_mesh;
    bool m_scheduledAccessDataTransfer; // Indicates if the device supports scheduled access data
                                         // transfer
    uint8_t m_macSecurity; // Indicates the MAC security level of the device
    uint8_t m_dlcServiceType; // Indicates the DLC service type of the device
    uint8_t m_rdPowerClass; // Indicates the RD power class of the device
    uint8_t m_maxNssFoRx; // Indicates the maximum number of spatial streams for reception
    uint8_t m_rxForTxDiversity; // Indicates if the device supports RX for TX diversity
    uint8_t m_rxGain; // Indicates the RX gain of the device
    uint8_t m_maxMcs; // Indicates the maximum MCS of the device
    uint8_t m_softBufferSize; // Indicates the size of the soft buffer of the device
    uint8_t m_numOfHarqProcesses; // Indicates the number of HARQ processes of the device
    uint8_t m_harqFeedbackDelay; // Indicates the HARQ feedback delay of the device
    bool m_dDelay; // Indicates if the device supports D-Delay
    bool m_halfDulp; // Indicates if the device supports half-duplex operation

    double m_txPowerDbm = 10; // Default transmission power in dBm

  private:
    void DoInitialize(void) override;

    // Membervariablen
    Ptr<Node> m_node;
    uint32_t m_ifIndex;
    Mac48Address m_address;
    uint16_t m_mtu;
    bool m_linkUp;
    NetDevice::ReceiveCallback m_rxCallback;
    NetDevice::PromiscReceiveCallback m_promiscRxCallback;
    Ptr<Dect2020Mac> m_mac;
    Ptr<Dect2020Phy> m_phy;
    TracedCallback<> m_linkChanges;

    uint8_t m_bandNumber; // Band Number (1-22)
};

} // namespace ns3

#endif /* DECT2020_NET_DEVICE_H */
