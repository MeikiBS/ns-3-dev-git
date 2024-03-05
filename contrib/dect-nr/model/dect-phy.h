#ifndef DECT_PHY_H
#define DECT_PHY_H

#include "ns3/object.h"
#include "ns3/simple-net-device.h"
#include "dect-channel.h"
#include "dect-channel.h"

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

    Ptr<SimpleNetDevice> GetDevice() const;
    Ptr<Dect2020Channel> GetChannel() const;

  protected:
    Ptr<SimpleNetDevice> m_device; //!< The net device this PHY is attached to.
    Ptr<Dect2020Channel> m_channel; //!< The channel this PHY is attached to.

}; // class DectPhy

} // namespace dect2020
} // namespace ns3
#endif /* DECT_PHY_H */

