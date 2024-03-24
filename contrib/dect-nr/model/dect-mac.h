#ifndef DECT_MAC_H
#define DECT_MAC_H

#include "ns3/log.h"
#include "ns3/object.h"

#include "ns3/point-to-point-net-device.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{
class DectMac : public Object
{
  public:
    static TypeId GetTypeId();
    DectMac();
    ~DectMac() override;

    Mac48Address m_macAddress; // Nur zum ausprobieren für Simple Channel/Device

}; // class DectMac

} // namespace dect2020
} // namespace ns3

#endif /* DECT_MAC_H */
