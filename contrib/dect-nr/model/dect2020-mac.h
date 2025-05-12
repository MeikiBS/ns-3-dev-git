#ifndef DECT2020_MAC_H
#define DECT2020_MAC_H

#include "dect2020-physical-header-field.h"

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

struct ChannelEvaluation
{
    uint16_t channelId;
    uint32_t free = 0;
    uint32_t possible = 0;
    uint32_t busy = 0;

    uint32_t Total() const
    {
        return free + possible + busy;
    }
};

// Structure to hold the context for subslot scanning
struct SubslotScanContext
{
    uint32_t channelId;
    uint32_t subslotCount; // Counter for how many Subslots have been measured
    ChannelEvaluation evaluation;
    std::function<void(const ChannelEvaluation&)>
        onComplete; // Callback to be called when the scan is complete (OperatingChannelSelection,
                    // LBT, ...)
};

/**
 * \brief DECT2020 MAC Klasse
 *
 * Diese Klasse implementiert die grundlegende MAC-Schichtfunktionalität
 * für das DECT2020 Modul.
 */
class Dect2020Mac : public Object
{
  public:
    enum Dect2020PacketType
    {
        DATA = 0,
        BEACON = 1,
        UNICAST = 2,
        RD_BROADCAST = 3
    };

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

    // Send a MAC PDU
    void Send(Ptr<Packet> packet, const Address& dest, Dect2020PacketType type);

    // Empfang von Paketen von der PHY-Schicht
    void ReceiveFromPhy(Ptr<Packet> packet);

    // Zugriff auf die Mac-Adresse des NetDevice
    Mac48Address GetAddress(void) const;

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

    void OperatingChannelSelection();
    void EvaluateAllChannels();

    Dect2020PhysicalHeaderField CreatePhysicalHeaderField();

    void StartSubslotScan(uint32_t channelId,
                          uint32_t numSubslots,
                          std::function<void(const ChannelEvaluation&)> onComplete);
    void ScheduleNextSubslotMeasurement(std::shared_ptr<SubslotScanContext> context,
                                        uint32_t numSubslots);

    uint32_t m_currentChannelId = 0; // Number of the Channel that is currently the main Channel

    uint8_t m_subcarrierScalingFactor = 1;       // Subcarrier Scaling Factor
    uint8_t m_fourierTransformScalingFactor = 1; // Fourier Transform Scaling Factor

    // ETSI TS 103 636-4 V2.1.1 #7.2-1
    const double RSSI_THRESHOLD_MIN = -85; // dBm
    const double RSSI_THRESHOLD_MAX = -50; // dBm TODO: RSSI_THRESHOLDMAX = -52 - max TX Power of RD
    const int SCAN_MEAS_DURATION = 24;     // slots
    const double SCAN_SUITABLE = 0.75;     // Threshold when an operating channel can be considered
                                           // fulfilling the operating conditions.
    const int SCAN_STATUS_VALID = 300;     // seconds

    std::map<uint32_t, ChannelEvaluation> m_scanEvaluations;
    uint32_t m_completedScans = 0;

  private:
    Dect2020PhysicalHeaderField CreatePhysicalHeaderField(uint8_t packetLengthType,
                                                          uint32_t packetLength);

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

    uint8_t m_mcs = 0; // Modulation and Coding Scheme
};

} // namespace ns3

#endif /* DECT2020_MAC_H */
