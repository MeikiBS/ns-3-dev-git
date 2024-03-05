#ifndef DECT_NET_DEVICE_H
#define DECT_NET_DEVICE_H


#include "ns3/log.h"
#include "ns3/simple-net-device.h"
#include "ns3/pointer.h"

#include "dect-net-device.h"
#include "dect-mac.h"
#include "dect-phy.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{

class DectNetDevice : public SimpleNetDevice
{
  public:
    static TypeId GetTypeId();
    DectNetDevice();
    ~DectNetDevice() override;

    void SetMac(Ptr<DectMac> mac);
    void SetPhy(Ptr<DectPhy> phy);


}; // class DectNetDevice

} // namespace dect2020
} // namespace ns3

#endif /* DECT_NET_DEVICE_H */

