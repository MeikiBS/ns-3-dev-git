#ifndef DECT_CHANNEL_H
#define DECT_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/net-device.h"
#include "ns3/dect-mac.h"
#include "ns3/dect-phy.h"
#include "ns3/packet.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{


class Dect2020Channel : public Channel
{
  public:
    // TypeId
    static TypeId GetTypeId();

    // Constructor and Destructor
    Dect2020Channel();
    ~Dect2020Channel() override;

    std::size_t GetNDevices() const override;
    Ptr<NetDevice> GetDevice(std::size_t i) const override;

    //Dect2020Channel(Ptr<PropagationLossModel> loss, Ptr<PropagationDelayModel> delay);

    void Add(Ptr<DectPhy> phy);
    void Add(Ptr<DectMac> mac);

    void Send(Ptr<DectPhy> sender, Ptr<Packet> packet, Time duration, double frequencyMHz) const;

  protected:
    /**
     * The vector containing the PHYs that are currently connected to the
     * channel.
     */
    std::vector<Ptr<DectPhy>> m_phyList;
    uint32_t m_id; //!< Channel id for this channel
};

} // namespace dect2020
} // namespace ns3

#endif /* DECT_NR_H */

