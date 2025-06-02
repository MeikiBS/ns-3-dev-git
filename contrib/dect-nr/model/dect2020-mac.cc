/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-mac.h"

#include "dect2020-channel-manager.h"
#include "dect2020-mac-common-header.h"
#include "dect2020-mac-header-type.h"
#include "dect2020-mac-messages.h"
#include "dect2020-net-device.h"
#include "dect2020-phy.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

#include <iomanip> // Für std::setw und std::setfill
#include <random>

NS_LOG_COMPONENT_DEFINE("Dect2020Mac");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Mac);

TypeId
Dect2020Mac::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020Mac")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020Mac>();
    return tid;
}

/**
 * \brief Constructor: Initializes internal state and MAC structures.
 */
Dect2020Mac::Dect2020Mac()
{
    NS_LOG_FUNCTION(this);

    InitializeDevice(); // Initialize the Device
}

Dect2020Mac::~Dect2020Mac()
{
    NS_LOG_FUNCTION(this);
}

/**
 * \brief Set the associated DECT NetDevice.
 * \param device Pointer to the DECT NetDevice.
 */
void
Dect2020Mac::SetNetDevice(Ptr<Dect2020NetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
}

/**
 * \brief Set the associated PHY layer and connect receive callback for incoming packets from the
 * physical layer.
 * \param phy Pointer to the DECT PHY object.
 */
void
Dect2020Mac::SetPhy(Ptr<Dect2020Phy> phy)
{
    NS_LOG_FUNCTION(this << phy);
    m_phy = phy;

    m_phy->SetReceiveCallback(MakeCallback(&Dect2020Mac::ReceiveFromPhy, this));
}

/**
 * \brief Placeholder for application-layer packet transmission.
 *
 * Currently unused. Intended to forward packets from the application layer
 * to the MAC for transmission via the PHY layer.
 *
 * \param packet The packet to send.
 * \param receiverLongRdId Destination Long RD ID.
 * \param type DECT packet type (e.g. BEACON, UNICAST).
 */
void
Dect2020Mac::Send(Ptr<Packet> packet, const Address& dest, Dect2020PacketType type)
{
    // Trace-Aufruf
    m_txPacketTrace(packet);
}

/**
 * \brief Entry point for all packets received from the PHY layer.
 *
 * This method handles incoming packets by parsing the PHY and MAC headers.
 * Based on the MAC header type, the packet is dispatched to the appropriate
 * handler (e.g. beacon, unicast). RSSI and PHY metadata are stored for further use.
 *
 * \param packet The received packet.
 * \param rssiDbm The received signal strength in dBm.
 */

void
Dect2020Mac::ReceiveFromPhy(Ptr<Packet> packet, double rssiDbm)
{
    NS_LOG_FUNCTION(this << packet);

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Mac::ReceiveFromPhy: 0x" << std::hex << this->GetLongRadioDeviceId()
                << std::dec << " received a Packet with UID " << packet->GetUid());

    if (packet->GetUid() == 164)
    {
        NS_LOG_INFO("FUCK");
    }
    // --- Physical Header Field ---
    Dect2020PHYControlFieldType1 physicalHeaderField;
    packet->RemoveHeader(physicalHeaderField);

    // --- MAC Header Type ---
    Dect2020MacHeaderType macHeaderType;
    packet->RemoveHeader(macHeaderType);

    // --- Beacon Header ---
    if (macHeaderType.GetMacHeaderTypeField() ==
        Dect2020MacHeaderType::MacHeaderTypeField::BEACON_HEADER)
    {
        // If the candidate has sent a beacon message before, we can update its entry in the FT
        // Candidate List
        FtCandidateInfo* ft = FindOrCreateFtCandidate(physicalHeaderField.GetTransmitterIdentity());
        ft->rssiDbm = rssiDbm;
        ft->receptionTime = Simulator::Now();
        ft->ftPhyHeaderField = physicalHeaderField;
        ft->shortFtId = physicalHeaderField.GetTransmitterIdentity();
        ft->shortNetworkId = physicalHeaderField.GetShortNetworkID();
        ft->clusterChannelId =
            this->GetCurrentChannelId(); // ReceiveFromPhy can only be called if the sent Packet is
                                         // on the current channel

        HandleBeaconPacket(packet, ft);
    }
    // --- Unicast Header ---
    else if (macHeaderType.GetMacHeaderTypeField() ==
             Dect2020MacHeaderType::MacHeaderTypeField::UNICAST_HEADER)
    {
        HandleUnicastPacket(packet);
    }

    // Weiterleitung des Pakets an das NetDevice
    m_device->Receive(packet);

    // Trace-Aufruf
    m_rxPacketTrace(packet);
}

/**
 * \brief Handles incoming beacon packets (Network or Cluster Beacons).
 *
 * Extracts the beacon header and MAC multiplexing header. Depending on the
 * beacon message type (network or cluster), the FT candidate information
 * is updated and channel switching may be triggered.
 *
 * - Stores metadata in the FT candidate structure
 * - Triggers channel switch to cluster channel if required
 * - Updates internal state flags and statistics
 *
 * \param packet The beacon packet with remaining headers.
 * \param ft Pointer to the FT candidate this beacon came from.
 */
void
Dect2020Mac::HandleBeaconPacket(Ptr<Packet> packet, FtCandidateInfo* ft)
{
    if (packet->GetUid() == 164)
    {
        NS_LOG_INFO("FUCK");
    }
    // --- Beacon Header ---
    Dect2020BeaconHeader beaconHeader;
    packet->RemoveHeader(beaconHeader);
    ft->ftBeaconHeader = beaconHeader;
    ft->networkId = beaconHeader.GetNetworkId();
    ft->longFtId = beaconHeader.GetTransmitterAddress();

    // --- MAC Multiplexing Header ---
    Dect2020MacMuxHeaderShortSduNoPayload muxHeader;
    packet->RemoveHeader(muxHeader);
    m_lastFtMacMuxHeader = muxHeader;
    IETypeFieldEncoding ieType = muxHeader.GetIeTypeFieldEncoding();

    // --- Network Beacon Message ---
    if (ieType == IETypeFieldEncoding::NETWORK_BEACON_MESSAGE)
    {
        Dect2020NetworkBeaconMessage networkBeaconMessage;
        packet->RemoveHeader(networkBeaconMessage);
        ft->ftNetworkBeaconMessage = networkBeaconMessage;

        // Get the cluster channel id from the network beacon message
        uint16_t ftBeaconNextClusterChannel = networkBeaconMessage.GetNextClusterChannel();
        ft->clusterChannelId = ftBeaconNextClusterChannel;

        // if this RD is not on the cluster channel, and the association status is not associated
        // nor association pending, and the device is not currently waiting for a Cluster Beacon
        // (has recently switched the channel because it received a network beacon), switch to the
        // cluster channel.
        if (GetCurrentChannelId() != ftBeaconNextClusterChannel &&
            m_associationStatus != ASSOCIATED && m_associationStatus != ASSOCIATION_PENDING &&
            !isWaitingForClusterBeacon)
        {
            isWaitingForClusterBeacon = true;

            SetCurrentChannelId(ftBeaconNextClusterChannel);
        }

        // Statistics
        Dect2020Statistics::IncrementNetworkBeaconReceptionCount();
    }
    // --- Cluster Beacon Message ---
    else if (ieType == IETypeFieldEncoding::CLUSTER_BEACON_MESSAGE)
    {
        // Cluster Beacon received --> Reset the flag
        isWaitingForClusterBeacon = false;

        // if the PT has received a Cluster Beacon and no Network Beacon before, we assume the
        // cluster Channel is the current Channel
        if (ft->ftNetworkBeaconMessage.GetNextClusterChannel() == 0 &&
            !ft->ftClusterBeaconMessage.GetNextChannelIncluded())
        {
            ft->ftNetworkBeaconMessage.SetNextClusterChannel(this->m_currentChannelId);
        }

        Dect2020ClusterBeaconMessage clusterBeaconMessage;
        packet->RemoveHeader(clusterBeaconMessage);
        ft->ftClusterBeaconMessage = clusterBeaconMessage;

        Dect2020RandomAccessResourceIE randomAccessResourceIE;
        packet->RemoveHeader(randomAccessResourceIE);
        ft->ftRandomAccessResourceIE = randomAccessResourceIE;

        EvaluateClusterBeacon(clusterBeaconMessage, randomAccessResourceIE, ft);

        // Statistics
        Dect2020Statistics::IncrementClusterBeaconReception();
    }
}

/**
 * \brief Handles incoming unicast MAC packets.
 *
 * Parses the unicast header and checks whether the message is addressed
 * to this device. If so, the message is further parsed based on its type.
 * Supports Association Request and Association Response messages.
 *
 * \param packet The received unicast packet.
 */

void
Dect2020Mac::HandleUnicastPacket(Ptr<Packet> packet)
{
    // --- Unicast Header ---
    Dect2020UnicastHeader unicastHeader;
    packet->RemoveHeader(unicastHeader);

    // Check if the Unicast Message is for this device
    uint32_t receiverAddress = unicastHeader.GetReceiverAddress();
    uint32_t transmitterAddress = unicastHeader.GetTransmitterAddress();

    if (receiverAddress != this->GetLongRadioDeviceId())
    {
        return; // Packet is not for this device
    }

    // --- MAC Multiplexing Header ---
    Dect2020MacMuxHeaderShortSduNoPayload muxHeader;
    packet->RemoveHeader(muxHeader);

    IETypeFieldEncoding ieType = muxHeader.GetIeTypeFieldEncoding();

    // Message is an association request
    if (ieType == IETypeFieldEncoding::ASSOCIATION_REQUEST_MESSAGE)
    {
        // check whether this device is an FT --> a PT cannot process an association request
        if (this->m_device->GetTerminationPointType() !=
            Dect2020NetDevice::TerminationPointType::FT)
        {
            NS_LOG_WARN("Association Request received, but this device is not an FT.");
            return;
        }

        // Strip the message headers
        Dect2020AssociationRequestMessage associationRequestMessage;
        packet->RemoveHeader(associationRequestMessage);

        Dect2020RdCapabilityIE rdCapabilityIE;
        packet->RemoveHeader(rdCapabilityIE);

        Dect2020AssociationControlIE associationControlIE;
        packet->RemoveHeader(associationControlIE);

        // process the association request
        ProcessAssociationRequest(associationRequestMessage,
                                  rdCapabilityIE,
                                  associationControlIE,
                                  transmitterAddress);

        // Statistics
        Dect2020Statistics::IncrementAssociationRequestReceptionCount();
    }
    // Message is an association response
    else if (ieType == IETypeFieldEncoding::ASSOCIATION_RESPONSE_MESSAGE)
    {
        // check whether this device is an PT --> TODO: implement associationable FTs
        if (this->m_device->GetTerminationPointType() !=
            Dect2020NetDevice::TerminationPointType::PT)
        {
            NS_LOG_WARN("Association Response received, but this device is not an PT. FT "
                        "association not yet implemented.");
            return;
        }

        // Strip the message headers
        Dect2020AssociationResponseMessage associationResponseMessage;
        packet->RemoveHeader(associationResponseMessage);

        Dect2020RdCapabilityIE rdCapabilityIE;
        packet->RemoveHeader(rdCapabilityIE);

        ProcessAssociationResponse(associationResponseMessage, rdCapabilityIE, transmitterAddress);

        // Statistics
        Dect2020Statistics::IncrementAssociationResponseReceptionCount();
    }
}

/**
 * \brief Processes an Association Request received from a PT.
 *
 * Extracts and stores the PT's capabilities and requested flow parameters.
 * Schedules an Association Response based on HARQ timing requirements.
 *
 * Currently accepts all incoming requests without filtering.
 *
 * \param assoReqMsg Association Request message from the PT.
 * \param rdCapabilityIe Reported capabilities of the PT.
 * \param assoControlIe Association control settings (e.g. CB monitoring).
 * \param assoInitiatorLongRdId Long RDID of the requesting PT.
 */
void
Dect2020Mac::ProcessAssociationRequest(Dect2020AssociationRequestMessage assoReqMsg,
                                       Dect2020RdCapabilityIE rdCapabilityIe,
                                       Dect2020AssociationControlIE assoControlIe,
                                       uint32_t assoInitiatorLongRdId)
{
    AssociatedPtInfo ptInfo;
    ptInfo.associationEstablished = Simulator::Now();
    ptInfo.longRdId = assoInitiatorLongRdId;
    ptInfo.numberOfFlows = assoReqMsg.GetNumberOfFlows();
    ptInfo.powerConstraints = assoReqMsg.GetPowerConstraints();
    ptInfo.ftMode = assoReqMsg.GetFtMode();
    ptInfo.harqProcessesTx = assoReqMsg.GetHarqProcessesTx();
    ptInfo.maxHarqReTxDelay = assoReqMsg.GetMaxHarqReTxDelay();
    ptInfo.harqProcessesRx = assoReqMsg.GetHarqProcessesRx();
    ptInfo.maxHarqReRxDelay = assoReqMsg.GetMaxHarqReRxDelay();
    ptInfo.flowId = assoReqMsg.GetFlowId();
    ptInfo.cb_m = assoControlIe.GetClusterBeaconMonitoring() == 1 ? true : false;
    ptInfo.rdCapabilityIe = rdCapabilityIe;

    m_associatedPtDevices.push_back(ptInfo);

    // TODO: implement logic to accept or reject device. Currently every device is accepted

    // calculate the time to send the association response. DECT_Delay == 0 --> subslot n +
    // HARQ_feedback_delay +1
    uint32_t subslotsPerFrame = GetSubslotsPerSlot() * 24;

    uint32_t currentAbsSubslot = m_phy->GetCurrentAbsoluteSubslot();

    uint8_t harqDelay = rdCapabilityIe.GetHarqFeedbackDelay();
    uint32_t responseAbsSubslot;

    if (m_dectDelay == 0)
    {
        responseAbsSubslot = currentAbsSubslot + harqDelay + 1;
    }
    else
    {
        responseAbsSubslot = currentAbsSubslot + (subslotsPerFrame / 2); // 0.5 frame later
    }

    Time t = CalculcateTimeOffsetFromCurrentSubslot(responseAbsSubslot - currentAbsSubslot);

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Mac::ProcessAssociationRequest: Scheduling Association Response to 0x"
                << std::hex << ptInfo.longRdId << std::dec << " in "
                << (responseAbsSubslot - currentAbsSubslot) << " subslots and in "
                << t.GetMicroSeconds() << " us. Current AbsSubslot == " << currentAbsSubslot
                << " & response Subslot == " << responseAbsSubslot);

    Simulator::Schedule(t, &Dect2020Mac::SendAssociationResponse, this, ptInfo);
}

/**
 * \brief Processes an Association Response received from an FT.
 *
 * If the device is in ASSOCIATION_PENDING state and the response is accepted,
 * the MAC transitions to ASSOCIATED and stores the FT's identity.
 * Also sets a timer to verify association continuity.
 *
 * \param assoRespMsg The received Association Response message.
 * \param rdCapabilityIe Reported FT capabilities.
 * \param ftLongRdId Long RDID of the FT that sent the response.
 */
void
Dect2020Mac::ProcessAssociationResponse(Dect2020AssociationResponseMessage assoRespMsg,
                                        Dect2020RdCapabilityIE rdCapabilityIe,
                                        uint32_t ftLongRdId)
{
    if (m_associationStatus != ASSOCIATION_PENDING)
    {
        NS_LOG_WARN(
            "Association Response received, but association status is not ASSOCIATION_PENDING. "
            "Ignoring the response.");
        return;
    }

    if (assoRespMsg.GetAssociationAccepted())
    {
        // Currently, we accept all association responses
        m_associationStatus = ASSOCIATED;

        // If we dont receive a Cluster Beacon every 60 Seconds, we assume the FT is offline
        Simulator::Schedule(Seconds(60), &Dect2020Mac::VerifyAssociatedStatus, this);

        // Store the associated FTs Long RD ID
        m_associatedFTNetDeviceLongRdId = ftLongRdId;
        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Mac::HandleUnicastPacket: Association Response by 0x" << std::hex
                    << this->GetLongRadioDeviceId() << " accepted. ");

        // Statistics
        m_successfulAssociationTime = Simulator::Now();
    }
    else
    {
        // tbd
    }
}

/**
 * \brief Constructs and sends an Association Response to a requesting PT.
 *
 * Builds the complete response packet, including PHY and MAC headers,
 * the RD Capability IE, and the Association Response message.
 * The response is transmitted via the PHY to the specified PT.
 *
 * \param ptInfo Information about the PT that initiated the association.
 */
void
Dect2020Mac::SendAssociationResponse(AssociatedPtInfo ptInfo)
{
    Ptr<Packet> packet = Create<Packet>();

    // --- RD Capability IE ---
    // Create the RD Capability with the bootstrapping parameters of the FT, stored in the
    // Dect2020NetDevice object
    Dect2020RdCapabilityIE rdCapabilityIE;
    rdCapabilityIE.SetNumOfPhyCapabilities(this->m_device->m_numOfPHYCapabilities);
    rdCapabilityIE.SetRelease(this->m_device->m_release);
    rdCapabilityIE.SetGroupAssignment(this->m_device->m_supportGroupAssignment);
    rdCapabilityIE.SetPaging(this->m_device->m_supportPaging);
    rdCapabilityIE.SetOperatingModes(this->m_device->m_operatingModes);
    rdCapabilityIE.SetMesh(this->m_device->m_mesh);
    rdCapabilityIE.SetScheduledAccessDataTransfer(this->m_device->m_scheduledAccessDataTransfer);
    rdCapabilityIE.SetMacSecurity(this->m_device->m_macSecurity);
    rdCapabilityIE.SetDlcServiceType(this->m_device->m_dlcServiceType);
    rdCapabilityIE.SetRdPowerClass(this->m_device->m_rdPowerClass);
    rdCapabilityIE.SetMaxNssFoRx(this->m_device->m_maxNssFoRx);
    rdCapabilityIE.SetRxForTxDiversity(this->m_device->m_rxForTxDiversity);
    rdCapabilityIE.SetRxGain(this->m_device->m_rxGain);
    rdCapabilityIE.SetMaxMcs(this->m_device->m_maxMcs);
    rdCapabilityIE.SetSoftBufferSize(this->m_device->m_softBufferSize);
    rdCapabilityIE.SetNumOfHarqProcesses(this->m_device->m_numOfHarqProcesses);
    rdCapabilityIE.SetHarqFeedbackDelay(this->m_device->m_harqFeedbackDelay);
    rdCapabilityIE.SetDDelay(this->m_device->m_dDelay);
    rdCapabilityIE.SetHalfDulp(this->m_device->m_halfDulp);

    packet->AddHeader(rdCapabilityIE);

    // --- Association Response Message ---
    Dect2020AssociationResponseMessage associationResponseMessage;
    associationResponseMessage.SetAssociationAccepted(true);
    associationResponseMessage.SetHarqMod(0);
    associationResponseMessage.SetNumberOfFlows(7); // all flows accepted
    associationResponseMessage.SetGroupIdAndResourceTagIncluded(0);
    packet->AddHeader(associationResponseMessage);

    // --- MAC Multiplexing Header ---
    Dect2020MacMuxHeaderShortSduNoPayload muxHeader;
    muxHeader.SetMacExtensionFieldEncoding(0);
    muxHeader.SetLengthField(0);
    muxHeader.SetIeTypeFieldEncoding(IETypeFieldEncoding::ASSOCIATION_RESPONSE_MESSAGE);

    packet->AddHeader(muxHeader);

    // --- Unicast Header ---
    Dect2020UnicastHeader unicastHeader;
    unicastHeader.SetReset(0);
    unicastHeader.SetSequenceNumber(0);
    unicastHeader.SetReceiverAddress(ptInfo.longRdId);
    unicastHeader.SetTransmitterAddress(this->GetLongRadioDeviceId());

    packet->AddHeader(unicastHeader);

    // --- MAC Header Type ---
    Dect2020MacHeaderType macHeaderType;
    macHeaderType.SetVersion(0);
    macHeaderType.SetMacSecurity(Dect2020MacHeaderType::MacSecurityField::MAC_SECURITY_NOT_USED);
    macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::MacHeaderTypeField::UNICAST_HEADER);

    packet->AddHeader(macHeaderType);

    // --- Physical Header Field ---
    Dect2020PHYControlFieldType1 physicalHeaderField =
        CreatePhysicalHeaderField(1, packet->GetSize());
    physicalHeaderField.SetShortNetworkID(m_networkId);

    // Send the packet to the PHY
    m_phy->Send(packet, physicalHeaderField); // Send the packet to the PHY

    // Statistics
    Dect2020Statistics::IncrementAssociationResponseTransmissionCount();
}

/**
 * \brief Evaluates a received Cluster Beacon and prepares association if applicable.
 *
 * Updates the FT candidate with timing info from the beacon.
 * If the device is not associated, it enters PREPARING_SELECTION mode.
 * If the selected FT is identified, schedules the transmission of an Association Request
 * using randomized timing from the Random Access Resource IE.
 *
 * \param clusterBeaconMsg Received Cluster Beacon message.
 * \param rarIe Random Access Resource IE containing timing allocation.
 * \param ft Pointer to the corresponding FT candidate.
 */
void
Dect2020Mac::EvaluateClusterBeacon(const Dect2020ClusterBeaconMessage& clusterBeaconMsg,
                                   const Dect2020RandomAccessResourceIE& rarIe,
                                   FtCandidateInfo* ft)
{
    if(clusterBeaconMsg.GetSystemFrameNumber() == 135)
    {
        NS_LOG_INFO("FUCK");
    }
    ft->sfn = clusterBeaconMsg.GetSystemFrameNumber();

    m_lastSfn = clusterBeaconMsg.GetSystemFrameNumber();

    // if device is not associated, switch the status to preapring association and start the "select
    // best FT" timer
    if (m_associationStatus == AssociationStatus::NOT_ASSOCIATED)
    {
        // This is the time in seconds for the PT to listen to other channels to collect potential
        // FT candidates
        Time searchBestFtTimeSeconds = Seconds(2);

        // association status preparing means the device is scanning the network to find the best FT
        // candidate
        m_associationStatus = AssociationStatus::ASSOCIATION_PREPARING;
        DiscoverNetworks();
        Simulator::Schedule(searchBestFtTimeSeconds, &Dect2020Mac::SelectBestFtCandidate, this);
    }

    // prepare association request --> The device is waiting fot the cluster Beacon of the selected
    // FT
    if (m_associationStatus == AssociationStatus::WAITING_FOR_SELECTED_FT &&
        m_selectedFtCandidate.longFtId == ft->longFtId)
    {
        // Calculate the time from the Random Access Resource IE to send the association request
        uint16_t txSubslot;
        uint16_t startSubslot = rarIe.GetStartSubslot();
        bool lengthInSubslots = !rarIe.GetLengthType();
        uint8_t length = rarIe.GetRaraLength();

        // Select a random subslot to reduce the propability of collision
        std::random_device rd;
        std::mt19937 gen(rd());
        if (lengthInSubslots)
        {
            std::uniform_int_distribution<> dis(startSubslot, startSubslot + length - 1);
            txSubslot = dis(gen);

            if (txSubslot >= GetSubslotsPerSlot() * 24)
            {
                txSubslot = txSubslot % (GetSubslotsPerSlot() * 24);
                m_lastSfn++;
            }
        }
        else
        {
            // tbd --> Length in Slots
        }

        uint8_t slot = txSubslot / GetSubslotsPerSlot();
        uint8_t subslot = txSubslot % GetSubslotsPerSlot();

        Time t = m_phy->GetTimeToNextAbsoluteSubslot(m_lastSfn, slot, subslot);

        // Schedule the Association Request to be sent in the next subslot
        Simulator::Schedule(t, &Dect2020Mac::SendAssociationRequest, this, ft);
    }
}

/**
 * \brief Selects the best FT candidate based on RSSI and switches to its channel.
 *
 * Iterates through all discovered FT candidates and selects the one with the highest RSSI.
 * Updates internal state, switches to the candidate's cluster channel, and sets the MAC status
 * to WAITING_FOR_SELECTED_FT.
 *
 * Triggers a follow-up check after 2 seconds via VerifyWaitingForSelectedFtAssociationStatus().
 */
void
Dect2020Mac::SelectBestFtCandidate()
{
    NS_LOG_FUNCTION(this);

    // Step 1: Find the FT with the best (highest) RSSI-2
    double bestRssiDbm = -150.0; // start with very low RSSI
    FtCandidateInfo bestCandidate;
    bool found = false;

    for (const auto& ft : m_ftCandidates)
    {
        if (ft.rssiDbm > bestRssiDbm)
        {
            bestRssiDbm = ft.rssiDbm;
            NS_LOG_INFO("DEBUG: ft.clusterChannelId == " << ft.clusterChannelId);
            NS_LOG_INFO("DEBUG: ft.clusterChannelId == "
                        << ft.ftNetworkBeaconMessage.GetNextClusterChannel());
            bestCandidate = ft;
            found = true;
        }
    }

    if (!found)
    {
        NS_LOG_WARN("No valid FT candidate found. Staying in NOT_ASSOCIATED state.");
        m_associationStatus = AssociationStatus::NOT_ASSOCIATED;
        return;
    }

    // Step 2: Save the best FT info
    m_selectedFtCandidate = bestCandidate;

    // Step 3: Switch to the FT's channel
    SetCurrentChannelId(bestCandidate.clusterChannelId);

    // Step 4: Set status and wait for next cluster beacon of selected FT
    m_associationStatus = AssociationStatus::WAITING_FOR_SELECTED_FT;

    // Cluster Beacon triggers next step in EvaluateClusterBeacon
    // Verify after 2 Seconds if the device is still waiting for the selected FT. This should not
    // be the case because it should have received a Cluster Beacon and sent an Association Request
    // and the status is either associated oder association pending.
    Simulator::Schedule(Seconds(2),
                        &Dect2020Mac::VerifyWaitingForSelectedFtAssociationStatus,
                        this);
}

/**
 * \brief Returns the current MAC association status.
 *
 * \return Current AssociationStatus of the device (e.g. NOT_ASSOCIATED, ASSOCIATED).
 */
Dect2020Mac::AssociationStatus
Dect2020Mac::GetAssociationStatus() const
{
    return m_associationStatus;
}

/**
 * \brief Resets association status if still pending.
 *
 * If the device is still in ASSOCIATION_PENDING, switches back to NOT_ASSOCIATED.
 * This prevents getting stuck in a pending state.
 */
void
Dect2020Mac::VerifyPendingAssociationStatus()
{
    if (this->m_associationStatus == AssociationStatus::ASSOCIATION_PENDING)
    {
        m_associationStatus = AssociationStatus::NOT_ASSOCIATED;
    }
}

/**
 * \brief Fallback if no Cluster Beacon was received after FT selection.
 *
 * If the device is still waiting for the selected FT after a timeout,
 * the association status is reset to NOT_ASSOCIATED.
 */
void
Dect2020Mac::VerifyWaitingForSelectedFtAssociationStatus()
{
    if (this->m_associationStatus == AssociationStatus::WAITING_FOR_SELECTED_FT)
    {
        m_associationStatus = AssociationStatus::NOT_ASSOCIATED;
    }
}

/**
 * \brief Periodically verifies that the associated FT is still active.
 *
 * If no Cluster Beacon from the associated FT was received within the last 60 seconds,
 * the device resets its association status to NOT_ASSOCIATED.
 * Otherwise, the check is scheduled again.
 */
void
Dect2020Mac::VerifyAssociatedStatus()
{
    FtCandidateInfo* ft = FindFtCandidateByLongId(m_associatedFTNetDeviceLongRdId);
    Time lastrBeaconReceived = ft->receptionTime;
    Time currentTime = Simulator::Now();
    Time VerifyAssociatedStatusPeriod = Seconds(60);

    // Check if the last received Cluster Beacon is older than 60 seconds
    if (currentTime - lastrBeaconReceived > VerifyAssociatedStatusPeriod)
    {
        NS_LOG_WARN(
            Simulator::Now().GetMicroSeconds()
            << ": Dect2020Mac::VerifyAssociatedStatus: No Cluster Beacon received in the last "
            << "60 seconds. Setting association status to NOT_ASSOCIATED.");
        m_associationStatus = AssociationStatus::NOT_ASSOCIATED;
    }
    else
    {
        // Check again in VerifyAssociatedStatusPeriod
        Simulator::Schedule(VerifyAssociatedStatusPeriod,
                            &Dect2020Mac::VerifyAssociatedStatus,
                            this);
    }
}

/**
 * \brief Constructs and sends an Association Request to a selected FT.
 *
 * Assembles a complete unicast packet with PHY/MAC headers, RD Capability IE,
 * and the Association Request message. Sets the association status to pending
 * and schedules a fallback if no response is received.
 *
 * \param ft Pointer to the selected FT candidate.
 */

void
Dect2020Mac::SendAssociationRequest(FtCandidateInfo* ft)
{
    // Set the Association Status to ASSOCIATION_PENDING
    m_associationStatus = AssociationStatus::ASSOCIATION_PENDING;
    Simulator::Schedule(MilliSeconds(200),
                        &Dect2020Mac::VerifyPendingAssociationStatus,
                        this); // Verify the association status after 200 ms

    Ptr<Packet> packet = Create<Packet>();

    // --- Association Control IE ---
    Dect2020AssociationControlIE associationControlIE;
    associationControlIE.SetClusterBeaconMonitoring(0);
    associationControlIE.SetDlDataReception(0);
    associationControlIE.SetUlPeriod(0);

    packet->AddHeader(associationControlIE);

    // --- RD Capability IE ---
    Dect2020RdCapabilityIE rdCapabilityIE;
    rdCapabilityIE.SetRelease(this->m_device->m_release);
    rdCapabilityIE.SetGroupAssignment(this->m_device->m_supportGroupAssignment);
    rdCapabilityIE.SetPaging(this->m_device->m_supportPaging);
    rdCapabilityIE.SetOperatingModes(this->m_device->m_operatingModes);
    rdCapabilityIE.SetMesh(this->m_device->m_mesh);
    rdCapabilityIE.SetScheduledAccessDataTransfer(this->m_device->m_scheduledAccessDataTransfer);
    rdCapabilityIE.SetMacSecurity(this->m_device->m_macSecurity);
    rdCapabilityIE.SetDlcServiceType(this->m_device->m_dlcServiceType);
    rdCapabilityIE.SetRdPowerClass(this->m_device->m_rdPowerClass);
    rdCapabilityIE.SetMaxNssFoRx(this->m_device->m_maxNssFoRx);
    rdCapabilityIE.SetRxForTxDiversity(this->m_device->m_rxForTxDiversity);
    rdCapabilityIE.SetRxGain(this->m_device->m_rxGain);
    rdCapabilityIE.SetMaxMcs(this->m_device->m_maxMcs);
    rdCapabilityIE.SetSoftBufferSize(this->m_device->m_softBufferSize);
    rdCapabilityIE.SetNumOfHarqProcesses(this->m_device->m_numOfHarqProcesses);
    rdCapabilityIE.SetHarqFeedbackDelay(this->m_device->m_harqFeedbackDelay);
    rdCapabilityIE.SetDDelay(this->m_device->m_dDelay);
    rdCapabilityIE.SetHalfDulp(this->m_device->m_halfDulp);

    packet->AddHeader(rdCapabilityIE);

    // --- Association Request Message ---
    Dect2020AssociationRequestMessage associationRequestMessage;
    associationRequestMessage.SetSetupCause(0); // Initial association
    associationRequestMessage.SetNumberOfFlows(0);
    associationRequestMessage.SetPowerConstraints(0);
    associationRequestMessage.SetFtMode(0);
    associationRequestMessage.SetCurrent(0);
    associationRequestMessage.SetHarqProcessesTx(0);
    associationRequestMessage.SetMaxHarqReTxDelay(0);
    associationRequestMessage.SetHarqProcessesRx(0);
    associationRequestMessage.SetMaxHarqReRxDelay(0);
    associationRequestMessage.SetFlowId(0);

    packet->AddHeader(associationRequestMessage);

    // --- MAC Multiplexing Header ---
    Dect2020MacMuxHeaderShortSduNoPayload muxHeader;
    muxHeader.SetMacExtensionFieldEncoding(0);
    muxHeader.SetLengthField(0);
    muxHeader.SetIeTypeFieldEncoding(IETypeFieldEncoding::ASSOCIATION_REQUEST_MESSAGE);

    packet->AddHeader(muxHeader);

    // --- Unicast Header ---
    Dect2020UnicastHeader unicastHeader;
    unicastHeader.SetReset(0);
    unicastHeader.SetSequenceNumber(0);
    unicastHeader.SetReceiverAddress(ft->longFtId);
    unicastHeader.SetTransmitterAddress(this->GetLongRadioDeviceId());

    packet->AddHeader(unicastHeader);

    // --- MAC Header Type ---
    Dect2020MacHeaderType macHeaderType;
    macHeaderType.SetVersion(0);
    macHeaderType.SetMacSecurity(Dect2020MacHeaderType::MacSecurityField::MAC_SECURITY_NOT_USED);
    macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::MacHeaderTypeField::UNICAST_HEADER);

    packet->AddHeader(macHeaderType);

    // --- Physical Header Field ---
    Dect2020PHYControlFieldType1 physicalHeaderField =
        CreatePhysicalHeaderField(1, packet->GetSize());
    physicalHeaderField.SetShortNetworkID(m_potentialShortNetworkId);

    m_phy->Send(packet, physicalHeaderField); // Send the packet to the PHY

    // Statistics
    Dect2020Statistics::IncrementAssociationRequestTransmissionCount();
}

/**
 * \brief Calculates the simulation time offset for a future subslot.
 *
 * Converts a relative subslot delay into an absolute simulation Time,
 * using the current System Frame Number (SFN), slot, and subslot index.
 *
 * \param delayInSubslots Delay in absolute subslots from the current position.
 * \return Simulation time offset from now until the target subslot.
 */

Time
Dect2020Mac::CalculcateTimeOffsetFromCurrentSubslot(uint32_t delayInSubslots)
{
    uint32_t subslotsPerFrame = GetSubslotsPerSlot() * 24;
    uint32_t subslotsPerSlot = GetSubslotsPerSlot();
    uint32_t currentAbsSubslot = m_phy->GetCurrentAbsoluteSubslot();

    uint32_t responseAbsSubslot = currentAbsSubslot + delayInSubslots;

    uint8_t targetSfn = (m_phy->m_currentSfn + (responseAbsSubslot / subslotsPerFrame)) % 256;

    uint32_t subslotWithinFrame = responseAbsSubslot % subslotsPerFrame;
    uint32_t slot = subslotWithinFrame / subslotsPerSlot;
    uint32_t subslot = subslotWithinFrame % subslotsPerSlot;

    Time t = m_phy->GetTimeToNextAbsoluteSubslot(targetSfn, slot, subslot);

    return t;
}

/**
 * \brief Starts the MAC operation based on the device role (FT or PT).
 *
 * - FT devices initialize their own network (e.g. begin beaconing).
 * - PT devices begin network discovery after a short delay.
 */
void
Dect2020Mac::Start()
{
    if (this->m_device->GetTerminationPointType() == Dect2020NetDevice::TerminationPointType::FT)
    {
        InitializeNetwork();
    }
    else if (this->m_device->GetTerminationPointType() ==
             Dect2020NetDevice::TerminationPointType::PT)
    {
        m_deviceStartTime = Simulator::Now() - MilliSeconds(100);

        Simulator::Schedule(MilliSeconds(100),
                            &Dect2020Mac::DiscoverNetworks,
                            this); // Start Discovery
    }
}

/**
 * \brief Initializes a new DECT-2020 network (FT only).
 *
 * Generates a valid network ID and schedules the channel selection process.
 * Called when the device operates as a Fixed Termination (FT).
 */
void
Dect2020Mac::InitializeNetwork()
{
    uint32_t networkId = GenerateValidNetworkId();
    SetNetworkId(networkId);

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ":  FT-Device " << this << " started a new Network with the Network ID: "
                << std::hex << std::setw(8) << std::setfill('0') << networkId);

    Simulator::Schedule(Seconds(1), &Dect2020Mac::OperatingChannelSelection, this);
}

/**
 * \brief Initiates or continues the network discovery procedure for a PT device.
 *
 * This method simulates a periodic channel sweep by a portable termination (PT) device
 * to discover available fixed terminations (FTs) in a DECT-2020 NR network. It increases
 * the current operating channel by one and switches to that channel if it exists in the
 * current band. If the next channel is not available (e.g., end of band), the channel
 * list is wrapped and the PT starts scanning again from the beginning.
 *
 * The method reschedules itself after a fixed delay (e.g., 100 ms) until the device
 * becomes associated with an FT.
 *
 */
void
Dect2020Mac::DiscoverNetworks()
{
    if (m_associationStatus == AssociationStatus::ASSOCIATION_PENDING ||
        m_associationStatus == AssociationStatus::WAITING_FOR_SELECTED_FT)
    {
        return; // Device is in association process --> abort discovering networks
    }

    std::vector<Ptr<Dect2020Channel>> channelList =
        Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());

    if (m_associatedFTNetDeviceLongRdId != 0)
    {
        // return; // Device is already associated --> abort
    }

    // find the current channel
    uint32_t current = m_currentChannelId;
    uint32_t nextChannelId = 0;

    // search if current + 1 is part of the channel list
    bool found = false;
    for (auto& ch : channelList)
    {
        if (ch->m_channelId == current + 1)
        {
            nextChannelId = ch->m_channelId;
            found = true;
            break;
        }
    }

    if (!found)
    {
        // no valid next channel -> take the first one of the band
        nextChannelId = channelList.front()->m_channelId;
    }

    // Switch to the next channel
    SetCurrentChannelId(nextChannelId);

    Time t = MilliSeconds(1000); // Discover Network wait time
    Simulator::Schedule(t, &Dect2020Mac::DiscoverNetworks,
                        this); // Schedule next discovery
}

/**
 * \brief Starts and schedules periodic Cluster Beacon transmissions (FT only).
 *
 * Sends a Cluster Beacon on the current cluster channel and reschedules the next transmission.
 */
void
Dect2020Mac::StartClusterBeaconTransmission()
{
    SetCurrentChannelId(m_clusterChannelId);

    Ptr<Packet> clusterBeacon = BuildBeacon(true, m_clusterChannelId);
    m_phy->Send(clusterBeacon, CreatePhysicalHeaderField(1, clusterBeacon->GetSize()));

    // Schedule next cluster beacon transmission
    Simulator::Schedule(m_clusterBeaconPeriodTime,
                        &Dect2020Mac::StartClusterBeaconTransmission,
                        this);

    // Statistics
    Dect2020Statistics::IncrementClusterBeaconTransmission();
}

/**
 * \brief Builds a Random Access Resource Information Element for the Cluster Beacon.
 *
 * Specifies timing and access parameters for PTs wishing to associate.
 * Most control fields are currently set to fixed values.
 *
 * \return Configured Dect2020RandomAccessResourceIE.
 */
Dect2020RandomAccessResourceIE
Dect2020Mac::BuildRandomAccessResourceIE()
{
    Dect2020RandomAccessResourceIE randomAccessResourceIE;
    randomAccessResourceIE.SetRepeat(0);
    randomAccessResourceIE.SetSystemFrameNumberFieldIncluded(0);
    randomAccessResourceIE.SetChannelFieldIncluded(0);
    randomAccessResourceIE.SetSeparateChannelFieldIncluded(0);

    uint8_t m_startSubslot = CalculateAbsoluteStartSubslot();
    randomAccessResourceIE.SetStartSubslot(m_startSubslot);
    randomAccessResourceIE.SetLengthType(0); // length in subslots
    randomAccessResourceIE.SetRaraLength(m_device->m_randomAccessResourceLength);
    randomAccessResourceIE.SetMaxRachLengthType(0); // length in subslots
    randomAccessResourceIE.SetMaxRachLength(2);     // Max 2  subslots / transmission
    randomAccessResourceIE.SetCwMinSig(0);          // CW min 0 --> backoff not yet implemented
    randomAccessResourceIE.SetDectDelay(m_dectDelay);
    randomAccessResourceIE.SetResponseWindow(5);
    randomAccessResourceIE.SetCwMaxSig(0); // CW max 0 --> backoff not yet implemented

    return randomAccessResourceIE;
}

uint8_t
Dect2020Mac::CalculateAbsoluteStartSubslot()
{
    uint8_t absoluteStartSubslot = m_nextAvailableSubslot;
    uint8_t subslotsPerFrame = GetSubslotsPerSlot() * 24;

    // If the next available subslot is greater than the number of subslots per frame,
    // wrap around to the beginning of the frame
    if (absoluteStartSubslot >= subslotsPerFrame)
    {
        absoluteStartSubslot = absoluteStartSubslot % subslotsPerFrame;
        m_nextAvailableSubslot = absoluteStartSubslot;
    }

    m_nextAvailableSubslot += 2;

    return absoluteStartSubslot;
}

/**
 * \brief Starts a cyclic sweep to transmit Network Beacons on adjacent channels.
 *
 * Selects a set of valid channels spaced at fixed offsets (±2, ±4, ...) relative to
 * the cluster channel. Schedules beacon transmissions on each channel with time gaps
 * in between, then returns to the operating channel.
 *
 * This method reschedules itself for continuous sweeping.
 */
void
Dect2020Mac::StartNetworkBeaconSweep()
{
    std::vector<Ptr<Dect2020Channel>> channelList =
        Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());

    int16_t operatingChannel = static_cast<int16_t>(m_clusterChannelId);
    std::vector<int16_t> networkBeaconChannels;

    // the network beacon is also sent on the operating channel
    networkBeaconChannels.push_back(operatingChannel);

    // find valid channels with a higher channel id than the operating channel +2, +4, ...
    // this distance is needed if the device has a Bandwidth of 1.728 MHz. Needs to be adjusted for
    // other Bandwidths
    for (int16_t offset = 2;; offset += 2)
    {
        int16_t candidate = operatingChannel + offset;
        if (!Dect2020ChannelManager::ChannelExists(candidate))
        {
            break;
        }
        networkBeaconChannels.push_back(candidate);
    }

    // find valid channels with a lower channel id than the operating channel -2, -4, ...
    // this distance is needed if the device has a Bandwidth of 1.728 MHz. Needs to be adjusted for
    // other Bandwidths
    for (int16_t offset = 2;; offset += 2)
    {
        int16_t candidate = operatingChannel - offset;
        if (!Dect2020ChannelManager::ChannelExists(candidate))
        {
            break;
        }
        networkBeaconChannels.push_back(candidate);
    }

    // Schedule the network beacon transmission on the selected channels
    // Time beaconDuration = MicroSeconds(833);              // duration of the beacon transmission
    Time networkBeaconPeriod = m_networkBeaconPeriodTime; // gap between each transmission
    Time base = MilliSeconds(25); // Little offset to avoid dual TX Situations

    for (auto& channelId : networkBeaconChannels)
    {
        Simulator::Schedule(base, &Dect2020Mac::SendNetworkBeaconOnChannel, this, channelId);

        // Go back to the operating channel after beacon transmission
        // Simulator::Schedule(base + beaconDuration, &Dect2020Mac::ReturnToOperatingChannel, this);
        Simulator::Schedule(base, &Dect2020Mac::ReturnToOperatingChannel, this);

        base += networkBeaconPeriod;
    }

    // Schedule the next sweep
    // Simulator::Schedule(base + beaconDuration, &Dect2020Mac::StartNetworkBeaconSweep, this);
    Simulator::Schedule(base, &Dect2020Mac::StartNetworkBeaconSweep, this);
}

/**
 * \brief Switches the Device back to the cluster (operating) channel after beacon transmission.
 */
void
Dect2020Mac::ReturnToOperatingChannel()
{
    SetCurrentChannelId(m_clusterChannelId);
}

void
Dect2020Mac::SendNetworkBeaconOnChannel(uint16_t channelId)
{
    SetCurrentChannelId(channelId);

    Ptr<Packet> networkBeacon = BuildBeacon(false, channelId);

    m_phy->Send(networkBeacon, CreatePhysicalHeaderField(1, networkBeacon->GetSize()));

    // Statistics
    Dect2020Statistics::IncrementNetworkBeaconTransmissionCount();
}

/**
 * \brief Sends a Network Beacon on the specified channel.
 *
 * Used in the channel sweep to advertise the presence of the FT to PTs scanning nearby channels.
 *
 * \param isCluster Indicates if the wanted beacon is a Cluster Beacon (true) or a Network Beacon
 * (false).
 * \param channelId Channel ID on which the Network Beacon is transmitted.
 */
Ptr<Packet>
Dect2020Mac::BuildBeacon(bool isCluster, uint16_t networkBeaconTransmissionChannelId)
{
    Ptr<Packet> beacon = Create<Packet>();

    Dect2020MacMuxHeaderShortSduNoPayload muxHeader;
    muxHeader.SetMacExtensionFieldEncoding(0); // No length field included in the IE Header
    muxHeader.SetLengthField(0);               // No length field included in the IE Header

    if (isCluster)
    {
        // Set the IE Type Field in the MAC multiplexing Header
        muxHeader.SetIeTypeFieldEncoding(IETypeFieldEncoding::CLUSTER_BEACON_MESSAGE);

        // Cluster beacon message is always followed by the Random Access Ressource IE
        Dect2020RandomAccessResourceIE randomAccessResourceIE = BuildRandomAccessResourceIE();
        beacon->AddHeader(randomAccessResourceIE);

        Dect2020ClusterBeaconMessage msg;
        msg.SetSystemFrameNumber(this->m_phy->m_currentSfn);
        msg.SetTxPowerIncluded(0);
        msg.SetPowerConstraints(0);
        msg.SetFrameOffset(0);
        msg.SetNextChannelIncluded(0);
        msg.SetTimeToNextFieldPresent(0);
        msg.SetNetworkBeaconPeriod(m_networkBeaconPeriod);
        msg.SetClusterBeaconPeriod(m_clusterBeaconPeriod);
        msg.SetCountToTrigger(15);
        msg.SetRelativeQuality(0);
        msg.SetMinimumQuality(0);

        beacon->AddHeader(msg);
    }
    else
    {
        muxHeader.SetIeTypeFieldEncoding(IETypeFieldEncoding::NETWORK_BEACON_MESSAGE);

        Dect2020NetworkBeaconMessage msg;
        msg.SetTxPowerIncluded(1);
        msg.SetPowerConstraints(0);
        msg.SetCurrentClusterChannelIncluded(0);
        msg.SetNetworkBeaconChannels(0);
        msg.SetNetworkBeaconPeriod(m_networkBeaconPeriod);
        msg.SetClusterBeaconPeriod(m_clusterBeaconPeriod);
        msg.SetNextClusterChannel(m_clusterChannelId);
        // Cluster Channel ID is set in SendNetworkBeaconOnChannel()
        msg.SetTimeToNext(0); // TODO: Time to next = Next send time - current sim time

        beacon->AddHeader(msg);
    }

    // Add the mux header to the packet
    beacon->AddHeader(muxHeader);

    Dect2020BeaconHeader beaconHeader;
    beaconHeader.SetNetworkId(m_networkId);
    beaconHeader.SetTransmitterAddress(m_longRadioDeviceId);
    beacon->AddHeader(beaconHeader);

    Dect2020MacHeaderType macHeaderType;
    macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::BEACON_HEADER);
    beacon->AddHeader(macHeaderType);

    return beacon;
}

/**
 * \brief Starts both Cluster and Network Beacon transmissions.
 *
 * Called during FT initialization to begin beaconing on all relevant channels.
 */
void
Dect2020Mac::StartBeaconTransmission()
{
    StartNetworkBeaconSweep();
    StartClusterBeaconTransmission();
}

/**
 * \brief Initiates a subslot-wise RSSI scan on a specific channel.
 *
 * Measures channel power over multiple subslots and invokes a callback with the aggregated result.
 *
 * \param channelId Channel to scan.
 * \param numSubslots Number of subslots / slot in the current configuration.
 * \param onComplete Callback that receives the aggregated ChannelEvaluation result.
 */
void
Dect2020Mac::StartSubslotScan(uint32_t channelId,
                              uint32_t numSubslots,
                              std::function<void(const ChannelEvaluation&)> onComplete)
{
    auto context = std::make_shared<SubslotScanContext>();
    context->channelId = channelId;
    context->evaluation.channelId = channelId;
    context->onComplete = onComplete;
    context->subslotCount = 0;

    MeasureAndScheduleNextSubslot(context, numSubslots);
}

/**
 * \brief Measures RSSI for one subslot and schedules the next scan step.
 *
 * Categorizes the subslot as FREE, POSSIBLE, or BUSY based on RSSI thresholds like described in the
 * specification 106 636 4 5.1.2. Recursively schedules itself until the configured number of
 * subslots is scanned. Once complete, invokes the callback with the aggregated result.
 *
 * \param context Shared scan state (channel ID, counters, result storage).
 * \param numSubslots Total number of subslots to evaluate.
 */
void
Dect2020Mac::MeasureAndScheduleNextSubslot(std::shared_ptr<SubslotScanContext> context,
                                           uint32_t numSubslots)
{
    Subslot* subslot = m_phy->GetCurrentSubslot(context->channelId);

    double rssi = Dect2020ChannelManager::GetRssiDbm(context->channelId);

    if (rssi <= RSSI_THRESHOLD_MIN)
    {
        context->evaluation.free++;
    }
    else if (rssi <= RSSI_THRESHOLD_MAX)
    {
        context->evaluation.possible++;
    }
    else
    {
        context->evaluation.busy++;
    }

    context->subslotCount++;

    if (context->subslotCount < numSubslots)
    {
        // Schedule the next measurement
        Simulator::Schedule(NanoSeconds(subslot->subslotDurationNs),
                            &Dect2020Mac::MeasureAndScheduleNextSubslot,
                            this,
                            context,
                            numSubslots);
    }
    else
    {
        // Call the completion callback with the evaluation
        context->onComplete(context->evaluation);
    }
}

/**
 * \brief Returns the number of subslots per slot based on the subcarrier scaling factor.
 *
 * Used to determine subslot resolution for scheduling and channel access.
 *
 * \return Number of subslots per slot (2, 4, 8, or 16).
 */
uint8_t
Dect2020Mac::GetSubslotsPerSlot()
{
    uint32_t numSubslotsPerSlot = (m_subcarrierScalingFactor == 1)   ? 2
                                  : (m_subcarrierScalingFactor == 2) ? 4
                                  : (m_subcarrierScalingFactor == 4) ? 8
                                                                     : 16;

    return numSubslotsPerSlot;
}

/**
 * \brief Retrieves an FT candidate by its short ID or creates a new entry.
 *
 * Used when processing beacons to store RSSI, headers, and channel information.
 *
 * \param shortFtId The 16-bit short ID of the FT candidate.
 * \return Pointer to the matching or newly created FtCandidateInfo.
 */
FtCandidateInfo*
Dect2020Mac::FindOrCreateFtCandidate(uint16_t shortFtId)
{
    for (size_t idx = 0; idx < m_ftCandidates.size(); idx++)
    {
        if (m_ftCandidates[idx].shortFtId == shortFtId)
        {
            return &m_ftCandidates[idx];
        }
    }

    // No candidate found, create one
    FtCandidateInfo newCandidate;
    m_ftCandidates.push_back(newCandidate);

    return &m_ftCandidates.back();
}

/**
 * \brief Retrieves an FT candidate by its long RD ID or creates a new entry.
 *
 * Typically used after association to track beacon reception from the selected FT.
 *
 * \param longFtId The 32-bit long RD ID of the FT.
 * \return Pointer to the matching or newly created FtCandidateInfo.
 */
FtCandidateInfo*
Dect2020Mac::FindFtCandidateByLongId(uint32_t longFtId)
{
    for (size_t idx = 0; idx < m_ftCandidates.size(); idx++)
    {
        if (m_ftCandidates[idx].longFtId == longFtId)
        {
            return &m_ftCandidates[idx];
        }
    }

    // No candidate found, create one
    FtCandidateInfo newCandidate;
    m_ftCandidates.push_back(newCandidate);

    return &m_ftCandidates.back();
}

/**
 * \brief Starts subslot-wise RSSI scans on all valid channels to select the operating channel.
 *
 * Each scan is scheduled with an offset to avoid overlapping measurements.
 * Once all channels are scanned, triggers EvaluateAllChannels() to select the best one.
 */
void
Dect2020Mac::OperatingChannelSelection()
{
    m_scanEvaluations.clear();
    m_completedScans = 0;

    int numSubslots = GetSubslotsPerSlot() * 24;

    double subslotDurationNs = 208333.0;
    double totalScanTimeNs = subslotDurationNs * numSubslots;

    uint32_t channelOffset = 0;

    auto validChannels = Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());
    uint16_t numOfValidChannels = validChannels.size();
    for (auto& channel : validChannels)
    {
        Time delay = NanoSeconds(channelOffset * totalScanTimeNs);

        Simulator::Schedule(delay,
                            &Dect2020Mac::StartSubslotScan,
                            this,
                            channel->m_channelId,
                            numSubslots,
                            [this, numOfValidChannels](const ChannelEvaluation& eval) {
                                m_scanEvaluations[eval.channelId] = eval;
                                m_completedScans++;

                                if (m_completedScans == numOfValidChannels)
                                {
                                    EvaluateAllChannels();
                                }
                            });

        channelOffset++;
    }
}

/**
 * \brief Evaluates all scanned channels and selects the best operating channel.
 *
 * Selection strategy from ETSI 103 636 04 5.1.2:
 *  1. Prefer channels that are completely free.
 *  2. If none, select a channel with >= SCAN_SUITABLE threshold of usable subslots.
 *  3. Otherwise, pick the channel with the least busy and possible subslots.
 *
 * After selection, starts beacon transmission and schedules next scan.
 */

void
Dect2020Mac::EvaluateAllChannels()
{
    std::vector<ChannelEvaluation> evaluations;

    for (const auto& [channelId, eval] : m_scanEvaluations)
    {
        evaluations.push_back(eval);
    }

    // 1. Search 100 % free channels
    std::vector<ChannelEvaluation> freeChannels;
    for (const auto& e : evaluations)
    {
        uint32_t total = e.Total();
        if (e.free == total)
        {
            freeChannels.push_back(e);
        }
    }

    if (!freeChannels.empty())
    {
        Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
        uint32_t index = rand->GetInteger(0, freeChannels.size() - 1);
        ChannelEvaluation selected = freeChannels[index];

        m_clusterChannelId = selected.channelId;
        NS_LOG_INFO("Selected RANDOM COMPLETELY FREE channel: " << selected.channelId);

        // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
        Simulator::Schedule(Seconds(SCAN_STATUS_VALID),
                            &Dect2020Mac::OperatingChannelSelection,
                            this);
        // Start the beacon transmission
        StartBeaconTransmission();
        return;
    }

    // 2. Search channel with suitable conditions
    for (const auto& e : evaluations)
    {
        uint32_t total = e.Total();
        uint32_t suitable = e.free + e.possible;

        if (suitable >= static_cast<uint32_t>(total * SCAN_SUITABLE))
        {
            m_clusterChannelId = e.channelId;
            NS_LOG_INFO("Selected suitable channel: " << e.channelId);

            // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
            Simulator::Schedule(Seconds(SCAN_STATUS_VALID),
                                &Dect2020Mac::OperatingChannelSelection,
                                this);
            // Start the beacon transmission
            StartBeaconTransmission();
            return;
        }
    }

    // 3. Search channel with the lowest busy / lowest possible subslots
    uint32_t lowestBusy = std::numeric_limits<uint32_t>::max();
    uint32_t lowestPossible = std::numeric_limits<uint32_t>::max();
    uint32_t selectedChannelId = 0;

    for (const auto& e : evaluations)
    {
        // Number of busy subslots smaller --> better
        if (e.busy < lowestBusy)
        {
            lowestBusy = e.busy;
            lowestPossible = e.possible;
            selectedChannelId = e.channelId;
        }
        // Number of busy subslots equal --> number of possible subslots smaller (= number of free
        // subslots bigger) --> better
        else if (e.busy == lowestBusy)
        {
            if (e.possible < lowestPossible)
            {
                lowestPossible = e.possible;
                selectedChannelId = e.channelId;
            }
        }
    }

    m_clusterChannelId = selectedChannelId;

    // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
    Simulator::Schedule(Seconds(SCAN_STATUS_VALID), &Dect2020Mac::OperatingChannelSelection, this);
    // Start the beacon transmission
    StartBeaconTransmission();
}

/**
 * \brief Creates a PHY header for data messages based on packet size.
 * \param packetLengthType 0=subslots, 1=slots
 * \param packetLengthBytes Packet length in slots
 *
 * Computes number of slots needed to transmit the packet depending on current MCS.
 */
Dect2020PHYControlFieldType1
Dect2020Mac::CreatePhysicalHeaderField(uint8_t packetLengthType, uint32_t packetLengthBytes)
{
    Dect2020PHYControlFieldType1 physicalHeaderField;
    physicalHeaderField.SetPacketLengthType(packetLengthType); // 0 = in Subslots, 1 = in Slots

    if (packetLengthType == 0) // Packet length is given in subslots
    {
        // TBD
    }
    else if (packetLengthType == 1) // Packet length is given in slots
    {
        double packetLengthBits = packetLengthBytes * 8;

        double mcsTransportBlockSizeBits =
            this->m_phy->GetMcsTransportBlockSize(m_subcarrierScalingFactor,
                                                  m_fourierTransformScalingFactor,
                                                  m_mcs);

        uint16_t packetLengthInSlots = 0;
        packetLengthInSlots = std::ceil(packetLengthBits / mcsTransportBlockSizeBits);

        physicalHeaderField.SetPacketLength(
            packetLengthInSlots); // Size of packet in slots/subslots
    }

    physicalHeaderField.SetShortNetworkID(m_networkId);
    physicalHeaderField.SetTransmitterIdentity(m_shortRadioDeviceId);
    physicalHeaderField.SetTransmitPower(23); // TODO: Wie wird das bestimmt?
    physicalHeaderField.SetDFMCS(m_mcs);

    return physicalHeaderField;
}

/**
 * \brief Generates a valid 32-bit DECT network ID as described in ETSI 103 636-04 4.2.3.1.
 *
 * Ensures that:
 * - the 8 LSBs (used as short network ID) are not 0x00
 * - the remaining 24 MSBs are not all zero
 *
 * \return A valid non-zero network ID
 */

uint32_t
Dect2020Mac::GenerateValidNetworkId()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint32_t networkId = 0;
    uint8_t lsb;
    uint32_t msb;
    do
    {
        networkId = randomVar->GetValue(1, std::numeric_limits<uint32_t>::max());
        lsb = networkId & 0xFF;
        msb = (networkId >> 8) & 0xFFFFFF;
    } while (lsb == 0x00 || msb == 0x000000);

    return networkId;
}

/**
 * \brief Updates the current channel ID.
 *
 * Avoids redundant changes and initializes the cluster channel ID
 * if not yet set.
 *
 * \param channelId The channel to switch to
 */
void
Dect2020Mac::SetCurrentChannelId(uint32_t channelId)
{
    NS_LOG_FUNCTION(this << channelId);
    if (m_currentChannelId == channelId)
        return;

    m_currentChannelId = channelId;
    if (m_clusterChannelId == 0)
    {
        m_clusterChannelId = channelId;
    }
}

/**
 * \brief Returns the current operating channel ID.
 *
 * \return The current channel ID used by the device
 */
uint32_t
Dect2020Mac::GetCurrentChannelId() const
{
    return m_currentChannelId;
}

/**
 * \brief Sets the DECT Network ID for this device.
 *
 * Verifies that the short network ID (LSB) and the globally unique part (MSB) are valid.
 * Logs an error if either part is zero, as per ETSI TS 103 636-4 Section 4.2.3.1.
 *
 * \param networkId 32-bit network identifier to assign
 */

void
Dect2020Mac::SetNetworkId(uint32_t networkId)
{
    NS_LOG_FUNCTION(this << networkId);

    // Check for valid Network Ids
    uint8_t lsb = networkId & 0xFF;             // Short Network ID (last 8 Bit)
    uint32_t msb = (networkId >> 8) & 0xFFFFFF; // First 24 Bit of the Network ID (globally unique)

    if (lsb == 0x00 || msb == 0x000000)
    {
        NS_LOG_ERROR(Simulator::Now().GetMilliSeconds()
                     << ": Invalid Network ID: LSB and MSB have to be greater zero.");
        return;
    }

    m_networkId = networkId;
}

/**
 * \brief Returns the currently assigned network ID.
 *
 * \return 32-bit DECT network ID
 */

uint32_t
Dect2020Mac::GetNetworkId() const
{
    return m_networkId;
}

/**
 * \brief Generates a valid 32-bit Long Radio Device ID as described in ETSI 103 636-04
 * Section 4.2.2.3.
 *
 * Ensures the ID is within the allowed DECT NR range (1 to 0xFFFFFFFD),
 * excluding reserved values like 0x00000000, 0xFFFFFFFE, and 0xFFFFFFFF.
 *
 * \return A valid long RD ID
 */
uint32_t
Dect2020Mac::GenerateLongRadioDeviceId()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint32_t rdId = 0;

    do
    {
        rdId = randomVar->GetValue(1, 0xFFFFFFFD); // Range 0x00000001 bis 0xFFFFFFFD
    } while (rdId == 0x00000000 || rdId == 0xFFFFFFFE || rdId == 0xFFFFFFFF);

    return rdId;
}

/**
 * \brief Sets the long RD ID for this device.
 *
 * If an invalid value (0x00000000) is provided, a new valid ID is generated instead.
 *
 * \param rdId The 32-bit RD ID to assign
 */
void
Dect2020Mac::SetLongRadioDeviceId(uint32_t rdId)
{
    if (!(rdId == 0x00000000))
    {
        m_longRadioDeviceId = rdId;

        NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                    << ": Set Long Radio Device ID: 0x" << std::hex << rdId << " on Device "
                    << this);
    }
    else
    {
        NS_LOG_WARN(Simulator::Now().GetMilliSeconds()
                    << ": Invalid Long Radio Device ID detected. Generate a new one.");

        SetLongRadioDeviceId(GenerateLongRadioDeviceId());
    }
}

/**
 * \brief Returns the long RD ID of this device.
 *
 * \return The 32-bit long RD ID
 */
uint32_t
Dect2020Mac::GetLongRadioDeviceId() const
{
    return m_longRadioDeviceId;
}

/**
 * \brief Generates a valid 16-bit Short Radio Device ID as described in ETSI 103 636-04
 * Section 4.2.3.3.
 *
 * The ID is chosen randomly in the valid DECT NR range [0x0001, 0xFFFE], excluding
 * reserved values like 0x0000 and 0xFFFF.
 *
 * \return A valid short RD ID
 */

uint16_t
Dect2020Mac::GenerateShortRadioDeviceId()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint16_t rdId = 0;

    do
    {
        rdId = randomVar->GetValue(1, 0xFFFE); // Range 0x0001 to 0xFFFE
    } while (rdId == 0x0000 || rdId == 0xFFFF);

    NS_LOG_DEBUG(Simulator::Now().GetMilliSeconds()
                 << ": Short Radio Device ID " << rdId << "generated.");

    return rdId;
}

/**
 * \brief Sets the short RD ID for this device.
 *
 * If an invalid value (0x0000) is provided, a new valid ID is generated automatically.
 *
 * \param rdId The 16-bit RD ID to assign
 */
void
Dect2020Mac::SetShortRadioDeviceId(uint16_t rdId)
{
    if (!(rdId == 0x0000))
    {
        m_shortRadioDeviceId = rdId;
    }
    else
    {
        NS_LOG_WARN(Simulator::Now().GetMilliSeconds()
                    << ": Invalid Short Radio Device ID detected. Generate a new one.");

        SetShortRadioDeviceId(GenerateShortRadioDeviceId());
    }
}

/**
 * \brief Returns the short RD ID assigned to this device.
 *
 * \return 16-bit Short Radio Device ID
 */
uint16_t
Dect2020Mac::GetShortRadioDeviceId() const
{
    return m_shortRadioDeviceId;
}

/**
 * \brief Initializes the device by assigning valid Long and Short RD IDs.
 *
 * This function is called during MAC initialization to ensure each device
 * has unique DECT-2020 NR identifiers.
 */
void
Dect2020Mac::InitializeDevice()
{
    SetLongRadioDeviceId(GenerateLongRadioDeviceId());
    SetShortRadioDeviceId(GenerateShortRadioDeviceId());
}

/**
 * \brief Converts RX gain index (0–8) to dB gain value.
 *
 * Index is mapped linearly to range −10 dB to +6 dB as defined in ETSI TS 103 636-4
 * Table 6.4.3.5-1. Returns 0 dB if the index is invalid (> 8).
 *
 * \param index RX gain index (0 to 8)
 * \return RX gain in dB
 */
double
Dect2020Mac::GetRxGainFromIndex(uint8_t index) const
{
    if (index > 8)
    {
        NS_LOG_WARN("Invalid RX gain index: " << static_cast<int>(index));
        return 0; // default fallback
    }

    return static_cast<int8_t>(-10 + index * 2);
}
} // namespace ns3
