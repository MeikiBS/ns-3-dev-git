#ifndef DECT2020_SPECTRUM_SIGNAL_PARAMETERS
#define DECT2020_SPECTRUM_SIGNAL_PARAMETERS

#include "ns3/antenna-model.h"
#include "ns3/packet.h"
#include "ns3/spectrum-phy.h"
#include "ns3/spectrum-signal-parameters.h"
#include "ns3/spectrum-value.h" // Include the header for SpectrumValue

/**
 * @file dect2020-spectrum-signal-parameters.h
 * @brief Defines signal parameter structure for DECT-2020 NR PHY layer transmissions.
 *
 * This header extends ns-3's SpectrumSignalParameters to include DECT-specific transmission
 * attributes such as the currently used channel ID and the pointer to the transmitted packet.
 *
 * It is used by the Dect2020Phy layer to transmit packets via ns-3's SpectrumChannel framework.
 */

namespace ns3
{

/**
 * @class Dect2020SpectrumSignalParameters
 * @brief Signal parameters used by the DECT-2020 PHY layer in SpectrumChannel transmissions.
 *
 * This class inherits from ns3::SpectrumSignalParameters and encapsulates:
 * - The packet to be transmitted over the spectrum channel
 * - The DECT-specific channel ID on which the transmission takes place
 *
 * These parameters are passed to ns-3's SpectrumChannel::StartTx() method
 * and received by other PHY entities through StartRx().
 *
 * This structure allows DECT-2020 NR devices to embed channel-specific transmission metadata
 * into the physical layer for RSSI calculation, collision detection, and subslot-based scheduling.
 */

class Dect2020SpectrumSignalParameters : public SpectrumSignalParameters
{
  public:
    Dect2020SpectrumSignalParameters();
    virtual ~Dect2020SpectrumSignalParameters();
    virtual Ptr<SpectrumSignalParameters> Copy() const override;

    Ptr<Packet> txPacket;
    uint32_t m_currentChannelId;

  private:
};

} // namespace ns3

#endif // DECT2020_SPECTRUM_SIGNAL_PARAMETERS
