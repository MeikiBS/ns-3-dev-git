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

Dect2020Mac::Dect2020Mac()
{
    NS_LOG_FUNCTION(this);

    InitializeDevice(); // Initialize the Device
}

Dect2020Mac::~Dect2020Mac()
{
    NS_LOG_FUNCTION(this);
}

void
Dect2020Mac::SetNetDevice(Ptr<Dect2020NetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
    m_address = Mac48Address::ConvertFrom(device->GetAddress());
}

void
Dect2020Mac::SetPhy(Ptr<Dect2020Phy> phy)
{
    NS_LOG_FUNCTION(this << phy);
    m_phy = phy;

    m_phy->SetReceiveCallback(MakeCallback(&Dect2020Mac::ReceiveFromPhy, this));
}

void
Dect2020Mac::Send(Ptr<Packet> packet, const Address& dest, Dect2020PacketType type)
{
    NS_LOG_FUNCTION(this << packet << dest << type);

    // // Hier können MAC-Header hinzugefügt werden
    // if (type == BEACON)
    // {
    //     Dect2020BeaconHeader beaconHeader;
    //     beaconHeader.SetNetworkId(this->GetNetworkId());
    //     beaconHeader.SetTransmitterAddress(this->GetLongRadioDeviceId());
    // }

    // Senden des Pakets über die PHY-Schicht
    // m_phy->Send(packet);

    // Trace-Aufruf
    m_txPacketTrace(packet);
}

void
Dect2020Mac::ReceiveFromPhy(Ptr<Packet> packet, double rssiDbm)
{
    NS_LOG_FUNCTION(this << packet);

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Mac::ReceiveFromPhy: 0x" << std::hex << this->GetLongRadioDeviceId()
                << std::dec << " received a Packet with UID " << packet->GetUid());

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

    // Dect2020BeaconHeader beaconHeader;
    // packet->RemoveHeader(beaconHeader);
    // // NS_LOG_INFO(beaconHeader);

    // // Jetzt sicher entfernen
    // Dect2020NetworkBeaconMessage beaconMessage;
    // packet->RemoveHeader(beaconMessage);
    // // NS_LOG_INFO(beaconMessage);

    // Weiterleitung des Pakets an das NetDevice
    m_device->Receive(packet);

    // Trace-Aufruf
    m_rxPacketTrace(packet);
}

void
Dect2020Mac::HandleBeaconPacket(Ptr<Packet> packet, FtCandidateInfo* ft)
{
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
        // NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
        //             << ": Dect2020Mac::HandleBeaconPacket: 0x" << std::hex <<
        //             GetLongRadioDeviceId()
        //             << " received an NETWORK_BEACON_MESSAGE");
        Dect2020NetworkBeaconMessage networkBeaconMessage;
        packet->RemoveHeader(networkBeaconMessage);
        ft->ftNetworkBeaconMessage = networkBeaconMessage;

        // Get the cluster channel id from the network beacon message
        uint16_t ftBeaconNextClusterChannel = networkBeaconMessage.GetNextClusterChannel();
        ft->clusterChannelId = ftBeaconNextClusterChannel;

        // if this RD is not on the cluster channel, and the association status is not associated
        // nor association pending, switch to the cluster channel.
        if (GetCurrentChannelId() != ftBeaconNextClusterChannel &&
            m_associationStatus != ASSOCIATED && m_associationStatus != ASSOCIATION_PENDING && !isWaitingForClusterBeacon)
        {
            NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                        << ": 0x" << std::hex << GetLongRadioDeviceId()
                        << " is switching from channel: " << std::dec << GetCurrentChannelId() << " to channel: " << std::dec << ftBeaconNextClusterChannel);
            
            isWaitingForClusterBeacon = true;
            // // TODO: Cluster Beacon Period anpassen
            // Simulator::Schedule(MilliSeconds(110), 
            //                     &Dect2020Mac::ResetIsWaitingForClusterBeaconFlag, this);
            SetCurrentChannelId(ftBeaconNextClusterChannel);
        }
    }
    // --- Cluster Beacon Message ---
    else if (ieType == IETypeFieldEncoding::CLUSTER_BEACON_MESSAGE)
    {
        isWaitingForClusterBeacon = false;
        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Mac::HandleBeaconPacket: 0x" << std::hex <<
                    GetLongRadioDeviceId()
                    << " received an CLUSTER_BEACON_MESSAGE");

        Dect2020Statistics::IncrementClusterBeaconReception();

        Dect2020ClusterBeaconMessage clusterBeaconMessage;
        packet->RemoveHeader(clusterBeaconMessage);
        ft->ftClusterBeaconMessage = clusterBeaconMessage;

        Dect2020RandomAccessResourceIE randomAccessResourceIE;
        packet->RemoveHeader(randomAccessResourceIE);
        ft->ftRandomAccessResourceIE = randomAccessResourceIE;

        EvaluateClusterBeacon(clusterBeaconMessage, randomAccessResourceIE, ft);
    }
}

void
Dect2020Mac::ResetIsWaitingForClusterBeaconFlag()
{
    isWaitingForClusterBeacon = false;
}

void
Dect2020Mac::HandleUnicastPacket(Ptr<Packet> packet)
{
    // --- Unicast Header ---
    Dect2020UnicastHeader unicastHeader;
    packet->RemoveHeader(unicastHeader);

    // Check if the Unicast Message is for me
    uint32_t receiverAddress = unicastHeader.GetReceiverAddress();
    uint32_t transmitterAddress = unicastHeader.GetTransmitterAddress();

    if (receiverAddress != this->GetLongRadioDeviceId())
    {
        return;
    }

    // --- MAC Multiplexing Header ---
    Dect2020MacMuxHeaderShortSduNoPayload muxHeader;
    packet->RemoveHeader(muxHeader);

    IETypeFieldEncoding ieType = muxHeader.GetIeTypeFieldEncoding();

    // Message is a an association request
    if (ieType == IETypeFieldEncoding::ASSOCIATION_REQUEST_MESSAGE)
    {
        // check whether this device is an FT --> a PT cannot process an association request
        if (this->m_device->GetTerminationPointType() !=
            Dect2020NetDevice::TerminationPointType::FT)
        {
            NS_LOG_WARN("Association Request received, but this device is not an FT.");
            return;
        }

        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Mac::HandleUnicastPacket: 0x" << std::hex << receiverAddress
                    << " received an Association Request from 0x" << std::hex << transmitterAddress
                    << " with UID " << std::dec << packet->GetUid());

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
    }
    // Message is an association response
    else if (ieType == IETypeFieldEncoding::ASSOCIATION_RESPONSE_MESSAGE)
    {
        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Mac::HandleUnicastPacket: 0x" << std::hex << receiverAddress
                    << " received an Association Response from 0x" << std::hex << transmitterAddress
                    << " with UID " << std::dec << packet->GetUid());

        // check whether this device is an PT --> TODO: implement associationable FTs
        if (this->m_device->GetTerminationPointType() !=
            Dect2020NetDevice::TerminationPointType::PT)
        {
            NS_LOG_WARN("Association Response received, but this device is not an PT. FT "
                        "association not yet implemented.");
            return;
        }

        Dect2020AssociationResponseMessage associationResponseMessage;
        packet->RemoveHeader(associationResponseMessage);

        if (associationResponseMessage.GetAssociationAccepted())
        {
            // tbd: for now --> set association status to ASSOCIATED
            m_associationStatus = ASSOCIATED;
            m_associatedFTNetDeviceLongRdId = transmitterAddress;
            NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                        << ": Dect2020Mac::HandleUnicastPacket: Association Response by 0x"
                        << std::hex << this->GetLongRadioDeviceId() << " accepted. ");
        }
        else
        {
            // tbd
        }
    }
}

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

void
Dect2020Mac::SendAssociationResponse(AssociatedPtInfo ptInfo)
{
    Ptr<Packet> packet = Create<Packet>();

    // --- RD Capability IE ---
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
    associationResponseMessage.SetGroupId(0);

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

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Mac::SendAssociationResponse: Sending Association Response to 0x"
                << std::hex << ptInfo.longRdId << " with UID " << std::dec << packet->GetUid());

    m_phy->Send(packet, physicalHeaderField); // Send the packet to the PHY
}

void
Dect2020Mac::HandleNetworkBeacon(Dect2020BeaconHeader beaconHeader,
                                 Dect2020NetworkBeaconMessage networkBeaconMsg,
                                 FtCandidateInfo* ft)
{
    NS_LOG_INFO(">> ENTER HandleNetworkBeacon");
}

void
Dect2020Mac::EvaluateClusterBeacon(const Dect2020ClusterBeaconMessage& clusterBeaconMsg,
                                   const Dect2020RandomAccessResourceIE& rarIe,
                                   FtCandidateInfo* ft)
{
    ft->sfn = clusterBeaconMsg.GetSystemFrameNumber();

    m_lastSfn = clusterBeaconMsg.GetSystemFrameNumber();

    // if device is not associated, switch the status to preapring association and start the "select
    // best FT" timer
    if (m_associationStatus == AssociationStatus::NOT_ASSOCIATED)
    {
        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Mac::EvaluateClusterBeacon: Device is not associated. 0x"
                    << std::hex << GetLongRadioDeviceId()
                    << " is starting to search for the best FT candidate.");

        uint8_t searchBestFtTimeSeconds = 2;
        // association status preparing means the device is scanning the network to find the best FT
        // candidate
        m_associationStatus = AssociationStatus::ASSOCIATION_PREPARING;
        DiscoverNetworks();
        Simulator::Schedule(Seconds(searchBestFtTimeSeconds),
                            &Dect2020Mac::SelectBestFtCandidate,
                            this);
    }

    // prepare association request
    if (m_associationStatus == AssociationStatus::WAITING_FOR_SELECTED_FT &&
        m_selectedFtCandidate.longFtId == ft->longFtId)
    {
        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": Dect2020Mac::EvaluateClusterBeacon: Device is not associated. 0x"
                    << std::hex << GetLongRadioDeviceId()
                    << " is preparing an association request to 0x" << ft->longFtId);

        uint16_t txSubslot;
        uint16_t startSubslot = rarIe.GetStartSubslot();
        bool lengthInSubslots = !rarIe.GetLengthType();
        uint8_t length = rarIe.GetRaraLength();

        std::random_device rd;
        std::mt19937 gen(rd());
        if (lengthInSubslots)
        {
            std::uniform_int_distribution<> dis(startSubslot, startSubslot + length - 1);
            txSubslot = dis(gen);

            if (txSubslot >= GetSubslotsPerSlot() * 24)
            {
                NS_LOG_INFO("txSubslot before = " << txSubslot);
                txSubslot = txSubslot % (GetSubslotsPerSlot() * 24);
                NS_LOG_INFO("txSubslot after = " << txSubslot);
                m_lastSfn++;
            }
        }
        else
        {
            // tbd
        }

        uint8_t slot = txSubslot / GetSubslotsPerSlot();
        uint8_t subslot = txSubslot % GetSubslotsPerSlot();

        Time t = m_phy->GetTimeToNextAbsoluteSubslot(m_lastSfn, slot, subslot);

        NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                    << ": DEBUG: Scheduling Association Request in " << t.GetMicroSeconds());

        Simulator::Schedule(t, &Dect2020Mac::SendAssociationRequest, this, ft);
        m_associationStatus = AssociationStatus::ASSOCIATION_PENDING;
        Simulator::Schedule(t + MilliSeconds(1000),
                            &Dect2020Mac::VerifyPendingAssociationStatus,
                            this); // Verify the association status after ~1 second
    }
}

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
    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": RD with Long RD ID 0x" << std::hex << GetLongRadioDeviceId()
                << " selected FT with Long RD ID " << std::hex << bestCandidate.longFtId
                << " and RSSI: " << bestRssiDbm << " dBm");

    // Step 3: Abort running Events from Dect2020Mac::DisoverNetworks
    // if (m_discoverNetworksEvent.IsRunning())
    // {
    //     NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
    //                 << ": Device with Long RD ID 0x" << std::hex << GetLongRadioDeviceId()
    //                 << ": Canceling scheduled DiscoverNetworks event after FT selection.");
    //     m_discoverNetworksEvent.Cancel();
    // }

    // Step 4: Switch to the FT's channel
    SetCurrentChannelId(bestCandidate.clusterChannelId);
    if (m_currentChannelId > 1700)
    {
        NS_LOG_INFO("FUCK");
    }
    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Device with Long RD ID 0x" << std::hex << GetLongRadioDeviceId() << std::dec
                << " switched to channel " << m_currentChannelId
                << " to wait for next Cluster Beacon.");

    // Step 5: Set status and wait for next cluster beacon of selected FT
    m_associationStatus = AssociationStatus::WAITING_FOR_SELECTED_FT;
    // Beacon triggers next step in EvaluateClusterBeacon
    Simulator::Schedule(Seconds(2),
                        &Dect2020Mac::VerifyWaitingForSelectedFtAssociationStatus,
                        this);

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Mac::SelectBestFtCandidate: 0x" << std::hex << GetLongRadioDeviceId()
                << " selected FT candidate with long ID 0x" << bestCandidate.longFtId);

    for (auto& ft : m_ftCandidates)
    {
        NS_LOG_INFO("FT Candidate: 0x" << std::hex << ft.longFtId << " with RSSI: " << std::dec
                                       << ft.rssiDbm
                                       << " dBm, Cluster Channel ID: " << ft.clusterChannelId);
    }
}

Dect2020Mac::AssociationStatus
Dect2020Mac::GetAssociationStatus() const
{
    return m_associationStatus;
}

void
Dect2020Mac::VerifyPendingAssociationStatus()
{
    if (this->m_associationStatus == AssociationStatus::ASSOCIATION_PENDING)
    {
        m_associationStatus = AssociationStatus::NOT_ASSOCIATED;
    }
}

void
Dect2020Mac::VerifyWaitingForSelectedFtAssociationStatus()
{
    if (this->m_associationStatus == AssociationStatus::WAITING_FOR_SELECTED_FT)
    {
        m_associationStatus = AssociationStatus::NOT_ASSOCIATED;
    }
}

void
Dect2020Mac::SendAssociationRequest(FtCandidateInfo* ft)
{
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

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": DEBUG: 0x" << std::hex << GetLongRadioDeviceId()
                << " is sending an Association Request with UID: " << std::dec << packet->GetUid());

    m_phy->Send(packet, physicalHeaderField); // Send the packet to the PHY
}

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

    // Time t = m_phy->GetAbsoluteSubslotTime(targetSfn, slot, subslot);
    Time t = m_phy->GetTimeToNextAbsoluteSubslot(targetSfn, slot, subslot);

    NS_LOG_INFO("Absolute Subslot Time = " << t.GetMicroSeconds());

    auto fff = GetShortRadioDeviceId();
    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": DEBUG: CalculcateTimeOffsetFromCurrentSubslot() for Device 0x" << std::hex
                << fff << std::dec << " with delayInSubslots = " << delayInSubslots
                << ", targetSfn = " << static_cast<int>(targetSfn) << ", slot = " << slot
                << ", subslot = " << subslot << ", t = " << t.GetMicroSeconds());

    return t;
}

Mac48Address
Dect2020Mac::GetAddress(void) const
{
    return m_address;
}

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
        // DEBUG: Network ID auf festen Wert setzen, später mit Beacon empfangen
        // m_networkId = 123456;
        // JoinNetwork(m_networkId);

        Simulator::Schedule(MilliSeconds(100),
                            &Dect2020Mac::DiscoverNetworks,
                            this); // Start Discovery
    }
}

void
Dect2020Mac::InitializeNetwork()
{
    uint32_t networkId = GenerateValidNetworkId();
    SetNetworkId(networkId);

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ":  FT-Device " << this << " started a new Network with the Network ID: "
                << std::hex << std::setw(8) << std::setfill('0') << networkId);

    Simulator::Schedule(Seconds(1), &Dect2020Mac::OperatingChannelSelection, this);
    // Simulator::Schedule(Seconds(1), &Dect2020Mac::StartBeaconTransmission, this);
    // OperatingChannelSelection();
    // StartBeaconTransmission();
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
        return; // Device is already associated --> abort
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

    SetCurrentChannelId(nextChannelId);
    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": PT-Device " << std::hex << "0x" << GetLongRadioDeviceId()
                << " is scanning channel: " << std::dec << m_currentChannelId);

    Time t = MilliSeconds(1000); // Discover Network wait time
    m_discoverNetworksEvent = Simulator::Schedule(t,
                                                  &Dect2020Mac::DiscoverNetworks,
                                                  this); // Schedule next discovery
}

void
Dect2020Mac::JoinNetwork(uint32_t networkId)
{
    SetNetworkId(networkId);

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": PT-Device joined a Network with the Network ID: " << std::hex << std::setw(8)
                << std::setfill('0') << networkId);

    SetShortRadioDeviceId(GenerateShortRadioDeviceId());
}

void
Dect2020Mac::StartClusterBeaconTransmission()
{
    SetCurrentChannelId(m_clusterChannelId);

    Ptr<Packet> clusterBeacon = BuildBeacon(true, m_clusterChannelId);
    m_phy->Send(clusterBeacon, CreatePhysicalHeaderField(1, clusterBeacon->GetSize()));

    NS_LOG_INFO(Simulator::Now().GetMicroSeconds()
                << ": Dect2020Mac::StartClusterBeaconTransmission: Device 0x" << std::hex
                << GetLongRadioDeviceId() << std::dec << " sent Cluster Beacon on Channel "
                << m_clusterChannelId << " with UID " << clusterBeacon->GetUid());

    // Schedule next cluster beacon transmission
    Simulator::Schedule(MilliSeconds(100), &Dect2020Mac::StartClusterBeaconTransmission, this);
}

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
    // m_nextAvailableSubslot += 2; // 2 Subslots / RD

    randomAccessResourceIE.SetLengthType(0); // length in subslots
    randomAccessResourceIE.SetRaraLength(10);
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

void
Dect2020Mac::StartNetworkBeaconSweep()
{
    std::vector<Ptr<Dect2020Channel>> channelList =
        Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());

    int16_t operatingChannel = static_cast<int16_t>(m_clusterChannelId);
    std::vector<int16_t> networkBeaconChannels;

    // the network beacon should be sent on the operating channel
    networkBeaconChannels.push_back(operatingChannel);

    // find valid channels with a higher channel id than the operating channel +2, +4, ...
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
    Time beaconDuration = MicroSeconds(1);       // duration of the beacon transmission
    Time networkBeaconPeriod = MilliSeconds(10); // gap between each transmission
    Time base = Seconds(0);

    for (auto& channelId : networkBeaconChannels)
    {
        Simulator::Schedule(base, &Dect2020Mac::SendNetworkBeaconOnChannel, this, channelId);

        // Go back to the operating channel after beacon transmission
        Simulator::Schedule(base + beaconDuration, &Dect2020Mac::ReturnToOperatingChannel, this);

        base += networkBeaconPeriod;
    }

    // Schedule the next sweep
    Simulator::Schedule(base + beaconDuration, &Dect2020Mac::StartNetworkBeaconSweep, this);
}

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

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Dect2020Mac::SendNetworkBeaconOnChannel: Device 0x" << std::hex << GetLongRadioDeviceId() << std::dec << " sent Network Beacon on Channel "
                << channelId << " with UID " << networkBeacon->GetUid());

    // auto subslotTime = m_phy->GetAbsoluteSubslotTime(m_phy->m_currentSfn, 5, 1).GetNanoSeconds();
    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Mac::SendNetworkBeaconOnChannel GetAbsoluteSubslotTime(currentSfn,
    //             "
    //                "Slot = 5, Subslot = 1) = "
    //             << subslotTime);
}

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

void
Dect2020Mac::StartBeaconTransmission()
{
    StartNetworkBeaconSweep();
    StartClusterBeaconTransmission();
}

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

    ScheduleNextSubslotMeasurement(context, numSubslots);
}

void
Dect2020Mac::ScheduleNextSubslotMeasurement(std::shared_ptr<SubslotScanContext> context,
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
                            &Dect2020Mac::ScheduleNextSubslotMeasurement,
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

uint8_t
Dect2020Mac::GetSubslotsPerSlot()
{
    uint32_t numSubslotsPerSlot = (m_subcarrierScalingFactor == 1)   ? 2
                                  : (m_subcarrierScalingFactor == 2) ? 4
                                  : (m_subcarrierScalingFactor == 4) ? 8
                                                                     : 16;

    return numSubslotsPerSlot;
}

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
    NS_LOG_INFO("Selected the channel with the lowest number of busy / possible subslots: "
                << m_clusterChannelId);

    // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
    Simulator::Schedule(Seconds(SCAN_STATUS_VALID), &Dect2020Mac::OperatingChannelSelection, this);
    // Start the beacon transmission
    StartBeaconTransmission();
}

Dect2020PHYControlFieldType1
Dect2020Mac::CreatePhysicalHeaderField()
{
    Dect2020PHYControlFieldType1 physicalHeaderField;
    physicalHeaderField.SetPacketLengthType(0); // 0 = in Subslots, 1 = in Slots
    physicalHeaderField.SetPacketLength(5);     // Size of packet in slots/subslots

    // Short Network ID: The last 8 LSB bits of the Network ID # ETSI 103 636 04 4.2.3.1
    uint8_t shortNetworkID = m_networkId & 0xFF;
    physicalHeaderField.SetShortNetworkID(shortNetworkID);
    physicalHeaderField.SetTransmitterIdentity(m_shortRadioDeviceId);
    physicalHeaderField.SetTransmitPower(3);
    physicalHeaderField.SetDFMCS(m_mcs);

    return physicalHeaderField;
}

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

uint32_t
Dect2020Mac::GetCurrentChannelId() const
{
    return m_currentChannelId;
}

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

    NS_LOG_DEBUG(Simulator::Now().GetMilliSeconds()
                 << ": Network ID set: 0x" << std::hex << std::setw(8) << std::setfill('0')
                 << m_networkId << " on Device " << this);
}

uint32_t
Dect2020Mac::GetNetworkId() const
{
    return m_networkId;
}

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
        // Hier Short RDID
    }
}

uint32_t
Dect2020Mac::GetLongRadioDeviceId() const
{
    return m_longRadioDeviceId;
}

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

uint16_t
Dect2020Mac::GetShortRadioDeviceId() const
{
    return m_shortRadioDeviceId;
}

void
Dect2020Mac::InitializeDevice()
{
    SetLongRadioDeviceId(GenerateLongRadioDeviceId());
    SetShortRadioDeviceId(GenerateShortRadioDeviceId());
}

int8_t
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
