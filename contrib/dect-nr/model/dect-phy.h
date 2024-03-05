#ifndef DECT_PHY_H
#define DECT_PHY_H

#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/dect-channel.h"

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

    Ptr<NetDevice> GetDevice() const;
    Ptr<ns3::dect2020::Dect2020Channel> GetChannel() const;

  protected:
    Ptr<NetDevice> m_device; //!< The net device this PHY is attached to.
    Ptr<Dect2020Channel> m_channel; //!< The channel this PHY is attached to.

}; // class DectPhy

} // namespace dect2020
} // namespace ns3
#endif /* DECT_PHY_H */

