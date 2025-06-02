/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dect2020-phy.h"

#include "dect2020-channel-manager.h"
#include "dect2020-channel.h"
#include "dect2020-mac.h"
#include "dect2020-net-device.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

#include <algorithm>
#include <iomanip>

// Kann später wieder weg, nur DEBUG:
#include <cstdlib>
#include <ctime>

NS_LOG_COMPONENT_DEFINE("Dect2020Phy");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(Dect2020Phy);

/**
 * \brief Transport Block Sizes for 1-slot, single-stream transmissions with mu=1 and beta=1.
 *
 * This vector maps each MCS index (0-11) to its corresponding transport block size in bits.
 * These values are based on the DECT-2020 NR PHY specification 103 636-03 Table C.2-2 and are used
 * to calculate how many bits can be transmitted per slot depending on the selected MCS.
 */
const std::vector<uint16_t> Dect2020Phy::m_singleSlotSingleStreamTransportBlockSizesMu1Beta1{
    136,  // MCS 0
    296,  // MCS 1
    456,  // MCS 2
    616,  // MCS 3
    936,  // MCS 4
    1256, // MCS 5
    1416, // MCS 6
    1576, // MCS 7
    1896, // MCS 8
    2040, // MCS 9
    2296, // MCS 10
    2552  // MCS 11
};

TypeId
Dect2020Phy::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020Phy")
                            .SetParent<Object>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020Phy>();
    return tid;
}

Dect2020Phy::Dect2020Phy()
    : m_currentSlot(0),
      m_currentSubslot(0)
{
    NS_LOG_FUNCTION(this);
}

Dect2020Phy::~Dect2020Phy()
{
    NS_LOG_FUNCTION(this);
}

/**
 * \brief Initializes the PHY layer and starts the frame timer.
 *
 * This method sets the initial DECT channel for the device and starts the PHY frame timer if it is
 * not already running.
 *
 * Specifically, it:
 * - Retrieves the first valid DECT channel based on the configured band number.
 * - Assigns this channel to `m_dect2020Channel`.
 * - Generates a pseudo-random initial channel ID (currently hardcoded with seed 13).
 * - Informs the MAC layer of the initial channel via `SetCurrentChannelId`.
 * - Starts the internal frame timer to drive PHY time structure (frame/slot/subslot).
 *
 * \note The random channel selection is currently deterministic (seed = 13).
 */
void
Dect2020Phy::Start()
{
    // Set a start channel
    Ptr<Dect2020NetDevice> device = DynamicCast<Dect2020NetDevice>(this->m_device);
    Ptr<Dect2020Channel> startChannel =
        Dect2020ChannelManager::GetValidChannels(device->GetBandNumber()).front();
    m_dect2020Channel = startChannel;

    // uint32_t seed = this->m_mac->GetLongRadioDeviceId();
    uint32_t seed = 13;
    std::srand(seed);
    uint16_t randomChannelId = 1657 + (std::rand() % 21);
    m_mac->SetCurrentChannelId(randomChannelId);

    if (!m_isFrameTimerRunning)
    {
        m_isFrameTimerRunning = true;
        StartFrameTimer();
    }
}

/**
 * \brief Sets the MAC layer instance connected to this PHY.
 * \param mac Pointer to the MAC layer.
 */
void
Dect2020Phy::SetMac(Ptr<Dect2020Mac> mac)
{
    NS_LOG_FUNCTION(this << mac);
    m_mac = mac;
}

/**
 * \brief Sets the network device associated with this PHY.
 * \param device Pointer to the NetDevice.
 */
void
Dect2020Phy::SetDevice(Ptr<NetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
}

/**
 * \brief Sets the SpectrumChannel used by this PHY and registers this PHY as receiver.
 * \param channel Pointer to the SpectrumChannel.
 */
void
Dect2020Phy::SetChannel(Ptr<SpectrumChannel> channel)
{
    NS_LOG_FUNCTION(this << channel);
    m_channel = channel;

    if (m_channel)
    {
        m_channel->AddRx(this);
    }
}

/**
 * \brief Returns the associated SpectrumChannel.
 * \return Pointer to the SpectrumChannel.
 */
Ptr<SpectrumChannel>
Dect2020Phy::GetChannel(void) const
{
    return m_channel;
}

/**
 * \brief Returns the associated network device.
 * \return Pointer to the NetDevice.
 */
Ptr<NetDevice>
Dect2020Phy::GetDevice(void) const
{
    return m_device;
}

/**
 * \brief Send a packet from the PHY layer to the SpectrumChannel.
 *
 * This function prepares and transmits a packet by creating the necessary transmission parameters,
 * including power spectral density (PSD), transmission duration, and channel information.
 * It also updates the channel occupancy state to allow RSSI-based collision detection and
 * schedules the transmission on the SpectrumChannel.
 *
 * \param packet The packet to send
 * \param physicalHeader The DECT2020 PHY control header Field Type 1
 */
void
Dect2020Phy::Send(Ptr<Packet> packet, Dect2020PHYControlFieldType1 physicalHeader)
{
    NS_ASSERT_MSG(packet, "Packet is null");
    NS_ASSERT_MSG(this->m_device, "m_device is null");
    NS_LOG_FUNCTION(this << packet);

    // Create and initialize the transmission parameters
    Ptr<Dect2020SpectrumSignalParameters> params = Create<Dect2020SpectrumSignalParameters>();
    params->txPhy = this;
    params->txPacket = packet->Copy(); // deep copy to avoid modifying original
    params->txPacket->AddHeader(physicalHeader);
    params->m_currentChannelId = this->m_mac->m_currentChannelId;

    // Calculate the TX duration based on the packet size and MCS
    Time duration = Time(NanoSeconds(CalculateTxDurationNs(physicalHeader)));
    params->duration = duration;

    // Retrieve spectrum model for current band
    uint8_t bandId = Dect2020ChannelManager::GetBandNumber(m_mac->m_currentChannelId);
    Ptr<const SpectrumModel> specModel = Dect2020ChannelManager::GetSpectrumModel(bandId);
    Ptr<SpectrumValue> psd = Create<SpectrumValue>(specModel);

    // Get the channelIndex --> this is the index of the channel used for psd
    uint16_t channelIndex = this->m_mac->m_currentChannelId -
                            Dect2020ChannelManager::GetFirstValidChannelNumber(bandId);

    // Set the PSD value for the current channel
    Ptr<Dect2020NetDevice> dectNetDevice = DynamicCast<Dect2020NetDevice>(this->m_device);
    double txPowerWatt = Dect2020ChannelManager::DbmToW(dectNetDevice->m_txPowerDbm);

    // Inform channel manager of active transmission power for RSSI tracking
    Dect2020ChannelManager::AddSpectrumPowerToChannel(this->m_mac->m_currentChannelId, txPowerWatt);

    for (uint32_t i = 0; i < psd->GetSpectrumModel()->GetNumBands(); ++i)
    {
        (*psd)[i] = 0.0; // Initialize all channels to 0.0
    }

    // Set power only on the current channel
    (*psd)[channelIndex] = txPowerWatt;
    params->psd = psd;

    // The rssiHoldTime keeps transmit power in the channel slightly longer after transmission ends.
    // This ensures that overlapping transmissions can still be detected as collisions via RSSI
    // checks.
    Time rssiHoldTime = MicroSeconds(100);
    // Remove the PSD value after the transmission
    Simulator::Schedule(duration + rssiHoldTime,
                        &Dect2020ChannelManager::RemoveSpectrumPowerFromChannel,
                        this->m_mac->m_currentChannelId,
                        txPowerWatt);

    // Start the transmission
    Simulator::Schedule(duration, &ns3::SpectrumChannel::StartTx, m_channel, params);

    // Trace-Aufruf
    m_phyTxBeginTrace(packet);

    // Statistics
    Dect2020Statistics::IncrementSumOfAllPacketsSent();
}

/**
 * \brief Set the callback to be invoked when a packet is received.
 *
 * \param cb Callback function with parameters: received packet and received power in dBm
 */
void
Dect2020Phy::SetReceiveCallback(Callback<void, Ptr<Packet>, double> cb)
{
    m_receiveCallback = cb;
}

/**
 * \brief Get the current absolute subslot index.
 *
 * This value increases continuously with time and wraps around after a full frame.
 *
 * \return Absolute subslot index as uint16_t
 */
uint16_t
Dect2020Phy::GetCurrentAbsoluteSubslot() const
{
    return m_currentAbsoluteSubslot;
}

/**
 * \brief Set the mobility model for this PHY layer.
 *
 * \param m Pointer to a MobilityModel representing the device's position and movement
 */
void
Dect2020Phy::SetMobility(Ptr<MobilityModel> m)
{
    m_mobilityModel = m;
}

/**
 * \brief Get the associated mobility model.
 *
 * \return Pointer to the MobilityModel
 */
Ptr<MobilityModel>
Dect2020Phy::GetMobility() const
{
    return m_mobilityModel;
}

/**
 * \brief Get the spectrum model used for reception.
 *
 * \return Pointer to the SpectrumModel
 */
Ptr<const SpectrumModel>
Dect2020Phy::GetRxSpectrumModel() const
{
    return m_spectrumModel;
}

/**
 * \brief Get the antenna object associated with this PHY.
 *
 * \return Pointer to the antenna (can be IsotropicAntennaModel or custom model)
 */
Ptr<Object>
Dect2020Phy::GetAntenna() const
{
    return m_antenna;
}

/**
 * \brief Set the antenna object for this PHY.
 *
 * \return Pointer to the antenna object
 */
void
Dect2020Phy::SetAntenna(Ptr<Object> antenna)
{
    m_antenna = antenna;
}

/**
 * @brief Begins the reception process of a DECT-2020 NR packet.
 *
 * This method is called by the SpectrumChannel when a transmission is received
 * on the currently tuned channel. It performs the following steps:
 * - Verifies if the packet was sent on the same channel the PHY is listening to.
 * - Converts the received power into dBm and compares it with the minimum RX sensitivity,
 *   adjusted by the device's RX gain index.
 * - Checks the overall channel RSSI to estimate potential collisions:
 *   - If RSSI is moderately high, a packet drop is determined probabilistically.
 *   - If RSSI is very high, a packet drop is assumed due to collision.
 * - If no drop condition is met, the packet is handed over to the MAC layer through a callback.
 *
 * @param params Parameters of the received signal, including power spectral density (PSD),
 *               channel ID, and the actual packet.
 */
void
Dect2020Phy::StartRx(Ptr<SpectrumSignalParameters> params)
{
    // TODO: check if the device is in RX State (--> need to implement a TX/RX State Machine)

    // Cast the generic SpectrumSignalParameters to our specific DECT-2020 parameters
    Ptr<Dect2020SpectrumSignalParameters> dectParams =
        DynamicCast<Dect2020SpectrumSignalParameters>(params);

    // Extract the channel index within the spectrum model
    uint16_t channelIndex =
        dectParams->m_currentChannelId -
        Dect2020ChannelManager::GetFirstValidChannelNumber(
            Dect2020ChannelManager::GetBandNumber(dectParams->m_currentChannelId));

    // Extract the received signal strength (in Watt and dBm) from the PSD
    double rssiPacket = (*params->psd)[channelIndex];
    double rssiPacketDbm = Dect2020ChannelManager::WToDbm(rssiPacket);

    // Calculate the minimum Rx sensitivity in dBm
    Ptr<Dect2020NetDevice> dectNetDevice = DynamicCast<Dect2020NetDevice>(this->m_device);
    // For Band 1 and 1.728 MHz bandwidth, RX sensitivity is -99.7 dBm (ETSI TS 103 636-2
    // Table 7.2-1)
    double minRxSensitivityDbm = -99.7 + this->m_mac->GetRxGainFromIndex(dectNetDevice->m_rxGain);

    // Drop packet if the PHY is not currently tuned to the correct channel
    if (dectParams->m_currentChannelId != this->m_mac->m_currentChannelId)
    {
        return;
    }

    // Drop packet if below minimum RX sensitivity
    if (rssiPacketDbm < minRxSensitivityDbm)
    {
        // Statistics
        Dect2020Statistics::IncrementPacketsDroppedLowRssi();
        std::string packetType = Dect2020Statistics::GetPacketType(dectParams->txPacket->GetUid());
        std::ostringstream oss;
        oss << "0x" << std::hex << m_mac->GetLongRadioDeviceId() << std::dec << " dropped a "
            << packetType << " with the Packet UID " << dectParams->txPacket->GetUid()
            << " due to low RSSI";
        std::string message = oss.str();
        Dect2020Statistics::LogToConsole(message);
        return;
    }

    // Estimate potential collisions based on total channel power (RSSI)
    auto rssiChannelDbm = Dect2020ChannelManager::GetRssiDbm(dectParams->m_currentChannelId);

    // If RSSI is in an ambiguous range, apply probabilistic drop
    if (rssiChannelDbm > 11.5 && rssiChannelDbm < 13.1)
    {
        Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable>();
        double rand = randVar->GetValue(0.0, 1.0);
        if (rand > 0.5)
        {
            // Statistics and Logging
            Dect2020Statistics::IncrementPacketsDroppedCollision();
            std::string packetType =
                Dect2020Statistics::GetPacketType(dectParams->txPacket->GetUid());
            std::ostringstream oss;
            oss << "0x" << std::hex << m_mac->GetLongRadioDeviceId() << std::dec << " dropped a "
                << packetType << " with the Packet UID " << dectParams->txPacket->GetUid()
                << " due to collision";
            std::string message = oss.str();
            Dect2020Statistics::LogToConsole(message);
            return;
        }
    }
    // If RSSI clearly indicates collision, drop packet
    else if (rssiChannelDbm > 13.1)
    {
        // Statistics and Logging
        Dect2020Statistics::IncrementPacketsDroppedCollision();
        std::string packetType = Dect2020Statistics::GetPacketType(dectParams->txPacket->GetUid());
        std::ostringstream oss;
        oss << "0x" << std::hex << m_mac->GetLongRadioDeviceId() << std::dec << " dropped a "
            << packetType << " with the Packet UID " << dectParams->txPacket->GetUid()
            << " due to collision";
        std::string message = oss.str();
        Dect2020Statistics::LogToConsole(message);
        return;
    }

    // Otherwise, receive packet successfully and pass it to the MAC
    m_receiveCallback(dectParams->txPacket, rssiPacketDbm);
}

/**
 * @brief Starts a periodic frame timer to simulate DECT-2020 NR slot-based timing.
 *
 * This method initializes the start of a new frame (10 ms duration) and schedules the processing
 * of each of the 24 slots within the frame. Each slot has a duration of approximately 0.41667 ms.
 * At the end of the frame, the timer is recursively scheduled again to simulate the next frame.
 *
 * This mechanism provides the PHY with a slot-aligned time base for:
 * - Frame/slot synchronization
 * - Slot-specific operations (e.g., transmission or sensing)
 * - Interfacing with the MAC layer for subslot timing
 */
void
Dect2020Phy::StartFrameTimer()
{
    double slotDuration = 416666;   // 0,41667 ms in ns
    uint16_t frameDuration = 10000; // 10 ms in µs

    // Mark the simulation time of the current frame's start
    m_frameStartTime = Simulator::Now();

    // Schedule all 24 slots within the current frame
    for (uint32_t slot = 0; slot < 24; slot++)
    {
        double slotStartTime = slot * slotDuration;

        Simulator::Schedule(NanoSeconds(slotStartTime),
                            &Dect2020Phy::ProcessSlot,
                            this,
                            slot,
                            slotStartTime);
    }

    // Schedule the start of the next frame after 10 ms
    Simulator::Schedule(MicroSeconds(frameDuration), &Dect2020Phy::StartFrameTimer, this);
}

/**
 * @brief Processes a single slot within a DECT-2020 NR frame.
 *
 * This method is triggered once per slot (24 per frame) and performs the following actions:
 * - Increments the System Frame Number (SFN) at the beginning of each new frame.
 * - Updates the internal slot counter.
 * - Schedules subslot processing based on the subcarrier scaling factor.
 *
 * @param slot The slot index within the current frame (0–23).
 * @param slotStartTime The relative start time of the slot in nanoseconds.
 */
void
Dect2020Phy::ProcessSlot(uint32_t slot, double slotStartTime)
{
    if (slot == 0)
    {
        // Increase the system frame number once per frame
        m_currentSfn = (m_currentSfn + 1) % 256;
    }
    m_currentSlot = slot;

    // Define number of subslots depending on subcarrier scaling
    uint8_t subcarrierScalingFactor = 1;
    uint32_t numSubslotsPerSlot = (subcarrierScalingFactor == 1)   ? 2
                                  : (subcarrierScalingFactor == 2) ? 4
                                  : (subcarrierScalingFactor == 4) ? 8
                                                                   : 16;

    // Subslot Duration in ns
    double subslotDuration = 416666 / numSubslotsPerSlot;

    // Schedule processing of each subslot in this slot
    for (uint32_t subslot = 0; subslot < numSubslotsPerSlot; subslot++)
    {
        Simulator::Schedule(NanoSeconds(subslot * subslotDuration),
                            &Dect2020Phy::ProcessSubslot,
                            this,
                            slot,
                            subslot);
    }
}

/**
 * @brief Processes an individual subslot within a slot.
 *
 * Updates internal state for the currently active subslot and resets the RSSI value
 * for the subslot on the currently used cluster channel.
 *
 * @param slotId The slot index this subslot belongs to (0–23).
 * @param subslotId The subslot index within the slot (0–1/3/7/15 depending on scaling).
 */
void
Dect2020Phy::ProcessSubslot(uint32_t slotId, uint32_t subslotId)
{
    m_currentSubslot = subslotId;
    m_currentAbsoluteSubslot = (slotId * 2) + subslotId;

    // Reset the RSSI of the current Subslot
    Subslot* subslot = GetCurrentSubslot(this->m_mac->m_clusterChannelId);
    (*subslot).rssi = 0;
}

/**
 * @brief Returns a pointer to the current Slot object.
 *
 * Searches the internal slot list for the slot matching m_currentSlot
 * and returns a pointer to it.
 *
 * @param channelId The ID of the channel (not used yet, for future extension).
 * @return Pointer to the current Slot, or nullptr if not found.
 */
Slot*
Dect2020Phy::GetCurrentSlot(uint32_t channelId)
{
    // Iterate over all slots in the current channel
    for (Slot& slot : m_dect2020Channel->m_slots)
    {
        if (slot.slotId == m_currentSlot)
        {
            return &slot; // Match found, return pointer
        }
    }

    return nullptr; // Slot not found
}

/**
 * @brief Returns a pointer to the current Subslot object.
 *
 * Iterates through the subslots of the current slot and returns the matching one.
 *
 * @param channelId The ID of the channel (not used yet, for future extension).
 * @return Pointer to the current Subslot, or nullptr if not found.
 */
Subslot*
Dect2020Phy::GetCurrentSubslot(uint32_t channelId)
{
    // Iterate over all slots in the current channel
    for (Slot& slot : m_dect2020Channel->m_slots)
    {
        if (slot.slotId == m_currentSlot)
        {
            // Within the current slot, find the active subslot
            for (Subslot& subslot : slot.subslots)
            {
                if (subslot.subslotId == m_currentSubslot)
                {
                    return &subslot; // Match found
                }
            }
        }
    }

    return nullptr; // Subslot not found
}

/**
 * @brief Returns the transport block size in bits for a given MCS index.
 *
 * Currently only supports single slot, single stream with mu = 1 and beta = 1.
 *
 * @param mu Subcarrier spacing scaling factor (currently only mu = 1 supported)
 * @param beta Time domain scaling factor (currently only beta = 1 supported)
 * @param mcsIndex Modulation and Coding Scheme index
 * @return Transport block size in bits, or 0 if parameters are unsupported
 */
uint16_t
Dect2020Phy::GetMcsTransportBlockSize(uint8_t mu, uint8_t beta, uint8_t mcsIndex)
{
    if (mu == 1 && beta == 1)
    {
        if (mcsIndex >= m_singleSlotSingleStreamTransportBlockSizesMu1Beta1.size())
        {
            NS_LOG_WARN("Invalid MCS index for (mu = 1, beta = 1)");
            return 0;
        }
        return m_singleSlotSingleStreamTransportBlockSizesMu1Beta1[mcsIndex];
    }

    NS_LOG_WARN("Unsupported (mu,beta) combination in GetMCSTransportBlockSize");
    return 0;
}

/**
 * @brief Calculates the transmission duration in nanoseconds for a given packet.
 *
 * Uses the packet length and its type (slot or subslot based) to compute the duration.
 * Currently only slot-based duration is supported.
 *
 * @param physicalHeaderField PHY control field containing length and type
 * @return Transmission duration in nanoseconds
 */
double
Dect2020Phy::CalculateTxDurationNs(Dect2020PHYControlFieldType1 physicalHeaderField)
{
    if (physicalHeaderField.GetPacketLengthType() == 0) // Packet length is given in subslots
    {
        // TBD
    }
    else if (physicalHeaderField.GetPacketLengthType() == 1) // Packet length is given in slots
    {
        return physicalHeaderField.GetPacketLength() * 416670; // 416.67 ns per slot
    }
    else
    {
        NS_LOG_WARN("Invalid packet length type");
        return 0;
    }

    return 0;
}

/**
 * @brief Calculates the time offset from the current subslot to a target subslot.
 *
 * This method computes the time required to reach a specified subslot (defined by SFN, slot, and
 * subslot) starting from the current PHY state. It assumes that the target subslot lies within the
 * same or next frame.
 *
 * @param targetSfn The System Frame Number (not yet used, currently only for consistency)
 * @param targetSlot The slot index within the frame (0–23)
 * @param targetSubslot The subslot index within the slot
 * @return Time duration until the target subslot (relative to current time)
 */
Time
Dect2020Phy::GetTimeToNextAbsoluteSubslot(uint8_t targetSfn,
                                          uint8_t targetSlot,
                                          uint8_t targetSubslot) const
{
    // uint8_t currentSfn = m_currentSfn;
    uint8_t currentSlot = m_currentSlot;
    uint8_t currentSubslot = m_currentSubslot;

    // Calculate the absolute subslot index for the current and target subslot
    int currentAbsoluteSubslotIndex = GetAbsoluteSubslotIndex(currentSlot, currentSubslot);
    int targetAbsoluteSubslotIndex = GetAbsoluteSubslotIndex(targetSlot, targetSubslot);

    // Calculate the difference in absolute subslot indices
    int subslotDifference = targetAbsoluteSubslotIndex - currentAbsoluteSubslotIndex;

    // If the target subslot is in the next frame, adjust the difference
    if (subslotDifference < 0)
    {
        // If the target subslot is in the next frame, add the total number of subslots in a frame
        uint32_t subslotsPerFrame = this->m_mac->GetSubslotsPerSlot() * 24; // 24 slots per frame
        subslotDifference += subslotsPerFrame;
    }

    // Calculate the time until the target subslot
    Time subslotDuration =
        NanoSeconds(416666 / this->m_mac->GetSubslotsPerSlot()); // Subslot Duration in ns
    Time timeUntilNextSubslot = subslotDifference * subslotDuration;

    return timeUntilNextSubslot;
}

/**
 * @brief Converts a slot and subslot index into an absolute subslot index within a frame.
 *
 * @param slot Slot index (0–23)
 * @param subslot Subslot index within the slot
 * @return Absolute subslot index (0–N), where N = 24 * subslotsPerSlot - 1
 */
int
Dect2020Phy::GetAbsoluteSubslotIndex(uint8_t slot, uint8_t subslot) const
{
    // Calculate the absolute subslot index
    uint8_t subslotsPerSlot = this->m_mac->GetSubslotsPerSlot();
    int absoluteSubslotIndex = (slot * subslotsPerSlot) + subslot;

    return absoluteSubslotIndex;
}

} // namespace ns3
