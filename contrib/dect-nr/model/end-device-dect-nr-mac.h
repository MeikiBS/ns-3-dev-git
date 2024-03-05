#ifndef END_DEVICE_DECT_NR_MAC_H
#define END_DEVICE_DECT_NR_MAC_H

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

#include "ns3/application.h"
#include "ns3/attribute.h"
#include "ns3/dect-mac.h"
#include "ns3/nstime.h"
#include "ns3/point-to-point-net-device.h"

#include <cstdint> // für uint8_t

namespace ns3
{
namespace dect2020
{

class EndDeviceDect2020Mac : public DectMac
{
  public:
    static TypeId GetTypeId();
    EndDeviceDect2020Mac();
    ~EndDeviceDect2020Mac() override;

    Mac48Address m_macAddress;  // Nur zum ausprobieren für Simple Channel/Device

  protected:
    uint8_t m_macVersion = 0;

    uint8_t m_macResetDataMacPdu;
    uint32_t m_macNetworkId;
    uint32_t m_macTransmitterAddress;

}; // class EndDeviceDect2020Mac

class Dect2020MacHeaderType : public Header
{
  public:
    TypeId GetTypeId();
    Dect2020MacHeaderType();
    ~Dect2020MacHeaderType() override;

    void SetMacSecurityField(uint8_t m_macSecurityField);
    void SetMacHeaderTypeField(uint8_t macHeaderTypeField);

  protected:
    uint8_t m_macSecurity;
    uint8_t m_macHeaderTypeField;

}; // class Dect2020MacHeaderType

class Dect2020MacCommonHeader : public Header
{
    Dect2020MacCommonHeader();
    ~Dect2020MacCommonHeader() override;

}; // class Dect2020MacCommonHeader

class Dect2020DataMacPduHeader : public Dect2020MacCommonHeader

{
  public:
    TypeId GetTypeId();
    Dect2020DataMacPduHeader();
    ~Dect2020DataMacPduHeader() override;

    void SetMacDataPduHeaderResetField(uint8_t m_macDataPduHeaderResetField);
    void SetMacDataPduHeaderSequenceNumberField(uint16_t m_macSequenceNumber);
};

class Dect2020MacMultiplexingHeader : public Header
{
  public:
    TypeId GetTypeId();
    Dect2020MacMultiplexingHeader();
    ~Dect2020MacMultiplexingHeader() override;

    void SetMacExtensionField(uint8_t m_macExtensionField);
    void SetMacInformationElementField(uint8_t m_macInformationElementField);

  protected:
    uint8_t m_macExtensionField; // 2
    uint8_t m_macInformationElementField;

}; // class Dect2020MacMultiplexingHeader

class Dect2020MacMultiplexingHeaderShortSdu : public Dect2020MacMultiplexingHeader
{
  public:
    TypeId GetTypeId();
    Dect2020MacMultiplexingHeaderShortSdu();
    ~Dect2020MacMultiplexingHeaderShortSdu() override;
};

} // namespace dect2020
} // namespace ns3

#endif /* END_DEVICE_DECT_NR_MAC_H */
