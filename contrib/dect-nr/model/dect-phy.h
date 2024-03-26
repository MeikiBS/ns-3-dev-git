#ifndef DECT_PHY_H
#define DECT_PHY_H

#include "ns3/object.h"
#include "ns3/simple-net-device.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include <ns3/spectrum-phy.h>

#include "dect-channel.h"
#include "dect-net-device.h"


// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{

class DectPhy : public Object
{
  public:
    static TypeId GetTypeId();
    DectPhy();
    ~DectPhy() override;

    void SetDevice(Ptr<NetDevice> device);
    Ptr<NetDevice> GetDevice() const;
    
    void SetChannel(Ptr<SpectrumChannel> channel);
    Ptr<SpectrumChannel> GetChannel() const;

  protected:
    Ptr<NetDevice> m_device;        //!< The net device this PHY is attached to.
    Ptr<SpectrumChannel> m_channel; //!< The channel this PHY is attached to.

}; // class DectPhy

} // namespace dect2020
} // namespace ns3
#endif /* DECT_PHY_H */

