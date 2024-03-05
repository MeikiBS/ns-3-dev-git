#ifndef DECT_CHANNEL_H
#define DECT_CHANNEL_H

#include "dect-mac.h"
#include "dect-phy.h"

#include "ns3/simple-net-device.h"
#include "ns3/packet.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/simple-channel.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{

class Dect2020Channel : public SimpleChannel
{
  public:
    // TypeId
    static TypeId GetTypeId();

    // Constructor and Destructor
    Dect2020Channel();
    ~Dect2020Channel() override;

    void Send(Ptr<Packet> p,
              uint16_t protocol,
              Mac48Address to,
              Mac48Address from,
              Ptr<SimpleNetDevice> sender) override;
    
    void Add(Ptr<SimpleNetDevice> device) override;
    void BlackList(Ptr<SimpleNetDevice> from, Ptr<SimpleNetDevice> to) override;
    void UnBlackList(Ptr<SimpleNetDevice> from, Ptr<SimpleNetDevice> to) override;

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
