#ifndef DECT2020_MAC_HEADER_TYPE
#define DECT2020_MAC_HEADER_TYPE

/**
 * \file dect2020-mac-header-type.h
 * \ingroup Dect2020Mac
 * \brief Defines the MAC Header Type field used in DECT-2020 NR MAC PDUs.
 *
 * Implements header type according to ETSI TS 103 636-4 Section 6.3.2.
 */


#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3
{

/**
 * \class Dect2020MacHeaderType
 * \brief Defined in ETSI TS 103 636-4 Section 6.3.2.
 */
class Dect2020MacHeaderType : public Header
{
  public:
    enum MacSecurityField // ETSI TS 103 636-4 V 1.51 #6.3.2-1
    {
        MAC_SECURITY_NOT_USED = 0,   // MAC security is not used for this MAC PDU.
        MAC_SECURITY_USED_NO_IE = 1, // MAC security is used and the MAC Security IE is not present.
        MAC_SECURITY_USED_WITH_IE = 2, // MAC is security used and a MAC Sec. Info IE is in MAC PDU
        RESERVED = 3                   // Reserved.
    };

    enum MacHeaderTypeField // ETSI TS 103 636-4 V 1.51 #6.3.2-2
    {
        DATA_MAC_PDU_HEADER = 0,    // DATA MAC PDU header as defined in Figure 6.3.3.1-1
        BEACON_HEADER = 1,          // Beacon Header as defined in Figure 6.3.3.2-1
        UNICAST_HEADER = 2,         // Unicast Header as defined in Figure 6.3.3.3-1
        RD_BROADCASTING_HEADER = 3, // RD Broadcasting Header as defined in Figure 6.3.3.4-1
        ESCAPE = 15                 // Escape
    };

    Dect2020MacHeaderType();
    virtual ~Dect2020MacHeaderType();

    void SetVersion(uint8_t version);
    uint8_t GetVersion() const;
    void SetMacSecurity(MacSecurityField macSecurity);
    MacSecurityField GetMacSecurity() const;
    void SetMacHeaderTypeField(MacHeaderTypeField macHeaderTypeField);
    MacHeaderTypeField GetMacHeaderTypeField() const;

    // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

  private:
    uint8_t m_version;
    MacSecurityField m_macSecurity;
    MacHeaderTypeField m_macHeaderTypeField;
};

} // namespace ns3

#endif // DECT2020_MAC_HEADER_TYPE
