#ifndef DECT2020_MAC_H
#define DECT2020_MAC_H

#include "dect2020-mac-common-header.h"
#include "dect2020-mac-header-type.h"
#include "dect2020-mac-information-elements.h"
#include "dect2020-mac-messages.h"
#include "dect2020-mac-multiplexing-header.h"
#include "dect2020-physical-header-field.h"
#include "dect2020-statistics.h"

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
 * \file dect2020-mac.h
 * \ingroup Dect2020Mac
 * \brief Implements the MAC layer functionality for DECT-2020 NR.
 *
 * Handles MAC procedures such as beacon transmission, association,
 * header creation, and MAC state machine logic based on ETSI TS 103 636-4.
 */

/**
 * \brief Stores subslot occupancy statistics for a single channel.
 *
 * Used during the Operating Channel Selection procedure to assess how busy a channel is.
 */
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

/**
 * \brief Stores information about a potential Fixed Termination (FT) candidate discovered during
 * beacon reception.
 *
 * Contains the latest headers and IEs received from the FT as well as signal-related metadata
 * such as RSSI and channel information.
 */
struct FtCandidateInfo
{
    uint8_t shortNetworkId; // Short Network ID of the FT candidate
    uint32_t networkId;     // Network ID of the FT candidate
    uint32_t shortFtId;     // Short RD ID of the FT candidate
    uint32_t longFtId;      // Long RD ID of the FT candidate

    uint16_t clusterChannelId;
    uint8_t sfn;
    Time receptionTime;
    double rssiDbm;

    Dect2020PHYControlFieldType1 ftPhyHeaderField;       // Last received Physical Header Field
    Dect2020BeaconHeader ftBeaconHeader;                 // Last received Beacon Header
    Dect2020UnicastHeader ftUnicastHeader;               // Last received Unicast Header
    Dect2020NetworkBeaconMessage ftNetworkBeaconMessage; // Last received Network Beacon Message
    Dect2020ClusterBeaconMessage ftClusterBeaconMessage; // Last received Cluster Beacon Message
    Dect2020RandomAccessResourceIE
        ftRandomAccessResourceIE; // Last received Random Access Resource IE
};

/**
 * \brief Stores association state and capabilities of an associated Portable Termination (PT).
 *
 * Used by an FT to manage all currently associated PTs and their supported features.
 */
struct AssociatedPtInfo
{
    Time associationEstablished;
    uint32_t longRdId; // Long Radio Device ID of the PT
    uint8_t numberOfFlows;
    bool powerConstraints;
    bool ftMode;
    uint8_t harqProcessesTx;
    uint8_t maxHarqReTxDelay;
    uint8_t harqProcessesRx;
    uint8_t maxHarqReRxDelay;
    uint8_t flowId;
    bool cb_m;
    Dect2020RdCapabilityIE rdCapabilityIe;

    double lastRssi;
};

/**
 * \brief Context structure used during subslot-based RSSI scanning.
 *
 * This struct is used to keep track of scanning progress during subslot measurements
 * on a specific channel. It accumulates the evaluation result and triggers a callback
 * once the scan is complete.
 */
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
 * \class Dect2020Mac
 * \brief DECT-2020 NR MAC layer implementation.
 *
 * Represents the Medium Access Control layer for a DECT-2020 NR node.
 * Manages transmission/reception of MAC PDUs, beaconing, association,
 * and MAC-PHY interaction.
 */
class Dect2020Mac : public Object
{
  public:
    /**
     * \brief Packet types used in DECT-2020 NR.
     *
     * This enumeration defines the logical classification of packet types
     * that can be transmitted or received within the MAC layer. ETSI 103 636-4 Section 6.3.3
     */
    enum Dect2020PacketType
    {
        DATA = 0,
        BEACON = 1,
        UNICAST = 2,
        RD_BROADCAST = 3
    };

    /**
     * \brief Association status of a device within the DECT-2020 NR network.
     *
     * These states define the current association state of a Radio Device (RD),
     * such as a Portable Termination (PT), with respect to a Fixed Termination (FT).
     */
    enum AssociationStatus
    {
        NOT_ASSOCIATED,
        ASSOCIATION_PREPARING,
        WAITING_FOR_SELECTED_FT,
        ASSOCIATION_PENDING,
        ASSOCIATED
    };

    static TypeId GetTypeId(void);

    Dect2020Mac();
    virtual ~Dect2020Mac();

    // Starts the MAC Layer
    void Start();

    // Initializes the Network (FT only)
    void InitializeNetwork();

    // Initialize the MAC-Layer of the Device
    void InitializeDevice();

    // Set the NetDevice and PHY for this MAC
    void SetNetDevice(Ptr<Dect2020NetDevice> device);
    void SetPhy(Ptr<Dect2020Phy> phy);

    // Receive a packet from the PHY layer
    void ReceiveFromPhy(Ptr<Packet> packet, double rssiDbm);

    // Starts the Network- and Cluster Beacon TX
    void StartBeaconTransmission();

    // Starts the Network Beacon TX
    void StartNetworkBeaconSweep();

    // Starts the Cluster Beacon TX
    void StartClusterBeaconTransmission();

    // Handle incoming Beacon and Unicast Messages
    void HandleBeaconPacket(Ptr<Packet> packet, FtCandidateInfo* ft);
    void HandleUnicastPacket(Ptr<Packet> packet);

    // Send a MAC PDU
    void Send(Ptr<Packet> packet, const Address& dest, Dect2020PacketType type);

    // Set/Get of channel ID (in DECT notation, n)
    void SetCurrentChannelId(uint32_t channelId);
    uint32_t GetCurrentChannelId() const;

    // Network ID Management
    uint32_t GenerateValidNetworkId();
    void SetNetworkId(uint32_t networkId);
    uint32_t GetNetworkId() const;

    // Long Radio Device ID Management
    uint32_t GenerateLongRadioDeviceId();
    void SetLongRadioDeviceId(uint32_t rdId);
    uint32_t GetLongRadioDeviceId() const;

    // Short Radio Device ID Management
    uint16_t GenerateShortRadioDeviceId();
    void SetShortRadioDeviceId(uint16_t rdId);
    uint16_t GetShortRadioDeviceId() const;

    // Operating Channel Selection
    void OperatingChannelSelection();
    void EvaluateAllChannels();

    // Build Messages
    Dect2020RandomAccessResourceIE BuildRandomAccessResourceIE();
    Ptr<Packet> BuildBeacon(bool isCluster, uint16_t networkBeaconTransmissionChannelId);

    // Association Procedure Management
    void EvaluateClusterBeacon(const Dect2020ClusterBeaconMessage& clusterBeaconMsg,
                               const Dect2020RandomAccessResourceIE& rarIe,
                               FtCandidateInfo* ft);

    // assoInitiatorLongRdId is the long radio device id of the RD which has sent the Association
    // Request
    void ProcessAssociationRequest(Dect2020AssociationRequestMessage assoReqMsg,
                                   Dect2020RdCapabilityIE rdCapabilityIe,
                                   Dect2020AssociationControlIE assoControlIe,
                                   uint32_t assoInitiatorLongRdId);
    void ProcessAssociationResponse(Dect2020AssociationResponseMessage assoRespMsg,
                                    Dect2020RdCapabilityIE rdCapabilityIe,
                                    uint32_t ftLongRdId);

    void StartSubslotScan(uint32_t channelId,
                          uint32_t numSubslots,
                          std::function<void(const ChannelEvaluation&)> onComplete);
    void MeasureAndScheduleNextSubslot(std::shared_ptr<SubslotScanContext> context,
                                       uint32_t numSubslots);
    FtCandidateInfo* FindOrCreateFtCandidate(uint16_t shortFtId);
    FtCandidateInfo* FindFtCandidateByLongId(uint32_t longFtId);
    void SendAssociationRequest(FtCandidateInfo* ft);
    void SendAssociationResponse(AssociatedPtInfo ptInfo);
    void VerifyPendingAssociationStatus();
    void VerifyWaitingForSelectedFtAssociationStatus();
    void VerifyAssociatedStatus();
    void SelectBestFtCandidate();
    AssociationStatus GetAssociationStatus() const;

    // Helper
    uint8_t GetSubslotsPerSlot();
    Time CalculcateTimeOffsetFromCurrentSubslot(uint32_t delayInSubslots);
    double GetRxGainFromIndex(uint8_t index) const;

    // ####################
    // ##### Variables ####
    // ####################

    // Waiting Flag when RD received a Network Beacon and is waiting for the Cluster Beacon
    bool isWaitingForClusterBeacon = false;

    // Number of the Channel that is currently the cluster Channel
    uint32_t m_clusterChannelId = 0;

    // Number of the Channel that the RD is currently connected with (e.g. for scanning/Beacon tx)
    uint32_t m_currentChannelId = 0;

    // Subcarrier Scaling Factor (mu)
    uint8_t m_subcarrierScalingFactor = 1;

    // Fourier Transform Scaling Factor (beta)
    uint8_t m_fourierTransformScalingFactor = 1;

    // DECT Delay
    bool m_dectDelay = false;

    // Network and Cluster Beacon Periods
    NetworkBeaconPeriod m_networkBeaconPeriod =
        NetworkBeaconPeriod::NETWORK_PERIOD_50MS; // Network Beacon Period
    ClusterBeaconPeriod m_clusterBeaconPeriod =
        ClusterBeaconPeriod::CLUSTER_PERIOD_100MS; // Cluster Beacon Period

    // Network and Cluster Beacon Periods as Time Objects --> TODO: Only use the upper ones and use
    // de "Dect2020NetworkBeaconMessage::GetNetworkBeaconPeriodTime()" Function
    Time m_networkBeaconPeriodTime = MilliSeconds(100); // Network Beacon Period
    Time m_clusterBeaconPeriodTime = MilliSeconds(100); // Cluster Beacon Period

    // Association Status of the Device
    AssociationStatus m_associationStatus = NOT_ASSOCIATED;

    // List of all FT candidates discovered during the association procedure
    std::vector<FtCandidateInfo> m_ftCandidates;

    // List of all associated Portable Termination (PT) devices. TODO: Implement logic to determine
    // whether the PT is really connected after the association process
    std::vector<AssociatedPtInfo> m_associatedPtDevices;

    // Last received MAC Multiplexing Header
    Dect2020MacMuxHeaderShortSduNoPayload m_lastFtMacMuxHeader;

    // Evaluations from the Operating Channel Selection procedure
    std::map<uint32_t, ChannelEvaluation> m_scanEvaluations;

    // Number of completed scans from the Operating Channel Selection procedure
    uint32_t m_completedScans = 0;

    // First subslot to be used by RD is 2 --> Subslot 0 and 1 are reserved for the cluster beacon
    // transmission
    uint8_t m_nextAvailableSubslot = 2;

    // Last System Frame Number (SFN) received from Cluster Beacon. TODO: This is not used yet, but
    // it should be used to synchronize the PT and FT Frame/Slot/Subslot Timer
    uint8_t m_lastSfn;

    // Short Network ID of the current FT candidate in the Association Procedure used in the PHY
    // Header
    uint8_t m_potentialShortNetworkId;

    // The selected FT candidate for association
    FtCandidateInfo m_selectedFtCandidate;

    // The associated FT NetDevice Long RD ID
    uint32_t m_associatedFTNetDeviceLongRdId = 0;

    // Statistic Variables
    Time m_deviceStartTime;           // Start time of the device
    Time m_successfulAssociationTime; // Time of the last successful association

    // ####################
    // ##### Constants ####
    // ####################

    // ETSI TS 103 636-4 V2.1.1 #7.2-1
    const double RSSI_THRESHOLD_MIN = -85; // dBm
    const double RSSI_THRESHOLD_MAX = -50; // dBm TODO: RSSI_THRESHOLDMAX = -52 - max TX Power of RD
    const int SCAN_MEAS_DURATION = 24;     // slots
    const double SCAN_SUITABLE = 0.75;     // Threshold when an operating channel can be considered
                                           // fulfilling the operating conditions.
    const int SCAN_STATUS_VALID = 300;     // seconds

  private:
    Dect2020PHYControlFieldType1 CreatePhysicalHeaderField(uint8_t packetLengthType,
                                                           uint32_t packetLength);

    void DiscoverNetworks();
    void SendNetworkBeaconOnChannel(uint16_t channelId);
    void ReturnToOperatingChannel();
    uint8_t CalculateAbsoluteStartSubslot();

    // Membervariablen
    Ptr<Dect2020NetDevice> m_device;
    std::vector<Dect2020NetDevice> m_associatedNetDevices; // List of associated NetDevices
    Ptr<Dect2020Phy> m_phy;

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
