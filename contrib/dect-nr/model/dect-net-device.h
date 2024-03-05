#ifndef DECT_NET_DEVICE_H
#define DECT_NET_DEVICE_H

#include "dect-mac.h"
#include "dect-net-device.h"
#include "dect-phy.h"

#include "ns3/error-model.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/queue.h"
#include "ns3/simple-net-device.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{

class DectPhy;
class Dect2020Channel;

class DectNetDevice : public SimpleNetDevice
{
  public:
    static TypeId GetTypeId();
    DectNetDevice();
    ~DectNetDevice() override;

    void SetMac(Ptr<DectMac> mac);
    void SetPhy(Ptr<DectPhy> phy);
    void SetChannel(Ptr<Dect2020Channel> channel);

  protected:
    Ptr<DectMac> m_mac;
    Ptr<DectPhy> m_phy;
    Ptr<Dect2020Channel> m_channel;

}; // class DectNetDevice

} // namespace dect2020
} // namespace ns3

#endif /* DECT_NET_DEVICE_H */
