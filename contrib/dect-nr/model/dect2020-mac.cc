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

    // TODO: Channel sucht sich ein RD selbst aus
    // m_clusterChannelId = 1658;

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
Dect2020Mac::ReceiveFromPhy(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);

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
        ft->receptionTime = Simulator::Now();
        ft->ftPhyHeaderField = physicalHeaderField;
        ft->shortFtId = physicalHeaderField.GetTransmitterIdentity();
        ft->shortNetworkId = physicalHeaderField.GetShortNetworkID();

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
    NS_LOG_INFO(">> ENTER HandleBeaconPacket with UID: " << packet->GetUid());

    // --- Beacon Header ---
    Dect2020BeaconHeader beaconHeader;
    packet->RemoveHeader(beaconHeader);
    ft->ftBeaconHeader = beaconHeader;
    ft->networkId = beaconHeader.GetNetworkId();
    ft->longFtId = beaconHeader.GetTransmitterAddress();
    NS_LOG_INFO("HandleBeaconPacket ft->longFtId == 0x" << std::hex << ft->longFtId);

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

        // if this RD is not on the cluster channel, switch to it. TODO: check association status
        // before
        if (GetCurrentChannelId() != ftBeaconNextClusterChannel)
        {
            NS_LOG_INFO("Switching to channel: " << ftBeaconNextClusterChannel
                                                 << " from channel: " << GetCurrentChannelId());
            SetCurrentChannelId(ftBeaconNextClusterChannel);
        }
    }
    // --- Cluster Beacon Message ---
    else if (ieType == IETypeFieldEncoding::CLUSTER_BEACON_MESSAGE)
    {
        // ########### Debugging ###########
        auto t = Simulator::Now().GetMilliSeconds();
        NS_LOG_INFO(t);
        // ########### Debugging ###########
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
        NS_LOG_INFO("Unicast message not for me, receiver address: 0x"
                    << std::hex << receiverAddress << ", my address: 0x"
                    << this->GetLongRadioDeviceId());
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

        Dect2020AssociationRequestMessage associationRequestMessage;
        packet->RemoveHeader(associationRequestMessage);

        Dect2020AssociationControlIE associationControlIE;
        packet->RemoveHeader(associationControlIE);

        // device is an FT --> process the association request
        ProcessAssociationRequest(associationRequestMessage,
                                  associationControlIE,
                                  transmitterAddress);
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
    }
}

void
Dect2020Mac::ProcessAssociationRequest(Dect2020AssociationRequestMessage assoReqMsg,
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

    // TODO: implement logic to accept or reject device. Currently every device is accepted
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

    if (m_associationStatus == AssociationStatus::NOT_ASSOCIATED)
    {
        uint16_t startSubslot = rarIe.GetStartSubslot();
        uint8_t slot = startSubslot / GetSubslotsPerSlot();
        uint8_t subslot = startSubslot % GetSubslotsPerSlot();

        Time t = m_phy->GetAbsoluteSubslotTime(m_lastSfn, slot, subslot);

        Simulator::Schedule(t, &Dect2020Mac::SendAssociationRequest, this, ft);
        m_associationStatus = AssociationStatus::ASSOCIATION_PENDING;

        NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                    << "Association Request an FT 0x" << std::hex << ft->longFtId << std::dec
                    << " geplant von 0x" << std::hex << this->GetLongRadioDeviceId() << std::dec
                    << " für Subslot " << static_cast<int>(subslot) << " in SFN "
                    << static_cast<int>(m_lastSfn));
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
    NS_LOG_INFO("DEBUG: ft->longFtId == 0x" << std::hex << ft->longFtId);
    unicastHeader.SetReceiverAddress(ft->longFtId);
    NS_LOG_INFO("DEBUG: unicastHeader == 0x" << std::hex << unicastHeader.GetReceiverAddress());

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

    NS_LOG_INFO("DEBUG: Sending Association Request to FT 0x" << std::hex << ft->longFtId
                                                              << " with UID: " << packet->GetUid());

    m_phy->Send(packet, physicalHeaderField); // Send the packet to the PHY
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
    if (m_associationStatus == AssociationStatus::ASSOCIATION_PENDING)
    {
        return; // Device is in association process --> abort discovering networks
    }
    std::vector<Ptr<Dect2020Channel>> channelList =
        Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());

    if (m_associatedFTNetDevice != nullptr)
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
    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": PT-Device " << std::hex << "0x" << GetShortRadioDeviceId()
                << " is scanning channel: " << std::dec << m_currentChannelId);

    Time t = MilliSeconds(1000); // Discover Network wait time
    Simulator::Schedule(t, &Dect2020Mac::DiscoverNetworks,
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

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Dect2020Mac::StartClusterBeaconTransmission sent Cluster Beacon on Channel "
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

    uint8_t m_startSubslot = m_nextAvailableSubslot;
    randomAccessResourceIE.SetStartSubslot(m_startSubslot);
    m_nextAvailableSubslot += 2; // 2 Subslots / RD

    randomAccessResourceIE.SetLengthType(0); // length in subslots
    randomAccessResourceIE.SetRaraLength(2);
    randomAccessResourceIE.SetMaxRachLengthType(0); // length in subslots
    randomAccessResourceIE.SetMaxRachLength(2);     // Max 2  subslots / transmission
    randomAccessResourceIE.SetCwMinSig(0);          // CW min 0 --> backoff not yet implemented
    randomAccessResourceIE.SetDectDelay(0);
    randomAccessResourceIE.SetResponseWindow(5);
    randomAccessResourceIE.SetCwMaxSig(0); // CW max 0 --> backoff not yet implemented

    return randomAccessResourceIE;
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

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Dect2020Mac::SendNetworkBeaconOnChannel sent Network Beacon on Channel "
    //             << channelId << " with UID " << networkBeacon->GetUid());

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
        msg.SetNextClusterChannel(networkBeaconTransmissionChannelId);
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
    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": Start Beacon Transmission on channel: " << m_clusterChannelId);

    // Ptr<Packet> networkBeacon = Create<Packet>();

    // // MAC Header Type
    // Dect2020MacHeaderType macHeaderType;
    // macHeaderType.SetMacHeaderTypeField(Dect2020MacHeaderType::BEACON_HEADER);
    // networkBeacon->AddHeader(macHeaderType);

    // // Mac Beacon Header
    // Dect2020BeaconHeader beaconHeader;
    // beaconHeader.SetNetworkId(m_networkId);
    // beaconHeader.SetTransmitterAddress(m_longRadioDeviceId);

    // networkBeacon->AddHeader(beaconHeader);

    // // MAC Beacon Message
    // Dect2020NetworkBeaconMessage beaconMessage;
    // // TODO: beaconMessage mit Inhalt füllen

    // networkBeacon->AddHeader(beaconMessage);

    // NS_LOG_INFO("Größe des Pakets direkt nach beaconMessage: " << networkBeacon->GetSize());

    // NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
    //             << ": StartBeaconTransmission() aufgerufen von 0x" << std::hex
    //             << this->GetLongRadioDeviceId() << " übergibt Paket mit der Größe " << std::dec
    //             << networkBeacon->GetSize() << " Bytes und UID " << networkBeacon->GetUid()
    //             << " an PHY.");

    // m_phy->Send(networkBeacon, CreatePhysicalHeaderField(1, networkBeacon->GetSize())); //

    // // NS_LOG_INFO("Network Beacon gesendet von Gerät 0x"
    // //             << std::hex  << this->GetLongRadioDeviceId());
    // // NS_LOG_INFO("MAC Header Type: " << macHeaderType.GetMacHeaderTypeField());

    // Simulator::Schedule(MilliSeconds(beaconMessage.GetNetworkBeaconPeriodTime()),
    //                     &Dect2020Mac::StartBeaconTransmission,
    //                     this);
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
            NS_LOG_INFO("DEBUG FindOrCreateFtCandidate: Found candidate with shortFtId = 0x"
                        << std::hex << shortFtId);
            return &m_ftCandidates[idx];
        }
    }

    // No candidate found, create one
    FtCandidateInfo newCandidate;
    m_ftCandidates.push_back(newCandidate);
    NS_LOG_INFO("Created new Candidate with ShortFtId = 0x" << std::hex << shortFtId);

    return &m_ftCandidates.back();
}

void
Dect2020Mac::OperatingChannelSelection()
{
    m_scanEvaluations.clear();
    m_completedScans = 0;

    int numSubslots = 48; // TODO: Get number of Subslots from configuration

    double subslotDurationNs = 208333.0;
    double totalScanTimeNs = subslotDurationNs * numSubslots;

    uint32_t channelOffset = 0;
    // for (auto& channel : m_phy->m_channels)
    auto validChannels = Dect2020ChannelManager::GetValidChannels(this->m_device->GetBandNumber());
    uint16_t numOfValidChannels = validChannels.size();
    for (auto& channel : validChannels)
    {
        Time delay = NanoSeconds(channelOffset * totalScanTimeNs);

        Simulator::Schedule(delay,
                            &Dect2020Mac::StartSubslotScan,
                            this,
                            channel->m_channelId,
                            48,
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

    // 1. Search 100 % free channel
    for (const auto& e : evaluations)
    {
        uint32_t total = e.Total();

        if (e.free == total)
        {
            m_clusterChannelId = e.channelId;
            NS_LOG_INFO("Selected COMPLETELY FREE channel: " << e.channelId);

            // Reschedule the next channel selection after SCAN_STATUS_VALID (300 seconds)
            Simulator::Schedule(Seconds(SCAN_STATUS_VALID),
                                &Dect2020Mac::OperatingChannelSelection,
                                this);
            // Start the beacon transmission
            StartBeaconTransmission();
            return;
        }
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

    // Short Network ID: The last 8 LSB bits of the Network ID # ETSI 103 636 04 4.2.3.1
    uint8_t shortNetworkID = m_networkId & 0xFF;
    physicalHeaderField.SetShortNetworkID(shortNetworkID);
    physicalHeaderField.SetTransmitterIdentity(m_shortRadioDeviceId);
    physicalHeaderField.SetTransmitPower(3); // TODO: Wie wird das bestimmt?
    physicalHeaderField.SetDFMCS(m_mcs);     // TODO: Wie wird das bestimmt?

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

    // DEBUG: networkId auf festen Wert setzen. Später random erstellen und PT Device Beacon
    // empfangen lassen
    networkId = 123456;
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

    NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                << ": Generated Long Radio Device ID: 0x" << std::hex << std::setw(8)
                << std::setfill('0') << rdId);
    return rdId;
}

void
Dect2020Mac::SetLongRadioDeviceId(uint32_t rdId)
{
    if (!(rdId == 0x00000000))
    {
        m_longRadioDeviceId = rdId;

        NS_LOG_INFO(Simulator::Now().GetMilliSeconds()
                    << ": Set Long Radio Device ID: 0x" << std::hex << std::setw(8)
                    << std::setfill('0') << rdId << " on Device " << this);
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
        rdId = randomVar->GetValue(1, 0xFFFE); // Range 0x0001 bis 0xFFFE
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
} // namespace ns3
