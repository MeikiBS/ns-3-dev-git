#ifndef DECT2020_MAC_H
#define DECT2020_MAC_H

#include "ns3/callback.h"
#include "ns3/mac48-address.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/random-variable-stream.h"
#include "ns3/traced-callback.h"

namespace ns3
{

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
    static TypeId GetTypeId(void);

    Dect2020Mac();
    virtual ~Dect2020Mac();

    /**
     * Starts the MAC-Layer
     */
    void Start();

    // Methoden zum Setzen der NetDevice und Phy
    void SetNetDevice(Ptr<Dect2020NetDevice> device);
    void SetPhy(Ptr<Dect2020Phy> phy);

    // Methoden zum Enqueue und Dequeue von Paketen
    void Enqueue(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

    // Empfang von Paketen von der PHY-Schicht
    void ReceiveFromPhy(Ptr<Packet> packet);

    // Zugriff auf die Mac-Adresse des NetDevice
    Address GetAddress(void) const;

    /**
     * Initialize a new Network
     */
    void InitializeNetwork();
    /**
     * Join a existing Network
     */
    /**
     * Generate a valid Network ID.
     */
    uint32_t GenerateValidNetworkId();
    /**
     * Join an existing Network.
     */
    void JoinNetwork(uint32_t networkId);
    /**
     * Set the Network ID.
     */
    void SetNetworkId(uint32_t networkId);
    /**
     * Get the Network ID.
     * \return the 32 Bit Network ID.
     */
    uint32_t GetNetworkId() const;
    /**
     * Start sending a beacon periodically.
     */
    void StartBeaconTransmission();
    /**
     * Generate the Long Radio Device ID as described in
     * ETSI TS 103 636-4 V 1.51 #4.2.3.2
     * \return 32 Bit Radio Device ID
     */
    uint32_t GenerateLongRadioDeviceId();
    /**
     * Set the Long Radio Device ID.
     */
    void SetLongRadioDeviceId(uint32_t rdId);
    /**
     * Get the Long Radio Device ID.
     */
    uint32_t GetLongRadioDeviceId() const;
    /**
     * Generate the Short Radio Device ID as described in
     * ETSI TS 103 636-4 V 1.51 #4.2.3.3
     * \return 16 Bit Radio Device ID
     */
    uint16_t GenerateShortRadioDeviceId();
    /**
     * Set the Short Radio Device ID.
     */
    void SetShortRadioDeviceId(uint16_t rdId);
    /**
     * Get the Short Radio Device ID.
     */
    uint16_t GetShortRadioDeviceId() const;
    /*
     * Initialize the MAC-Layer of the Device
     */
    void InitializeDevice();

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

    uint32_t m_networkId;          // ETSI TS 103 636-4 V 1.51 #4.2.3.1
    uint32_t m_longRadioDeviceId;  // ETSI TS 103 636-4 V 1.51 #4.2.3.2
    uint16_t m_shortRadioDeviceId; // ETSI TS 103 636-4 V 1.51 #4.2.3.3
};

} // namespace ns3

#endif /* DECT2020_MAC_H */