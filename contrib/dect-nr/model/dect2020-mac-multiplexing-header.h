#ifndef DECT2020_MUX_HEADER_H
#define DECT2020_MUX_HEADER_H

#include "ns3/header.h"

namespace ns3
{
enum IETypeFieldEncoding
{
    // # ETSI TS 103 636-4 V2.1.1 Table 6.3.4.2
    // Rest to be implemented
    NETWORK_BEACON_MESSAGE = 8,
    CLUSTER_BEACON_MESSAGE = 9,
    ASSOCIATION_REQUEST_MESSAGE = 10,
    ASSOCIATION_RESPONSE_MESSAGE = 11,
    RANDOM_ACCESS_RESOURCE_IE = 19
};

class Dect2020MacMuxHeaderShortSduNoPayload : public Header
{
  public:
    Dect2020MacMuxHeaderShortSduNoPayload();
    virtual ~Dect2020MacMuxHeaderShortSduNoPayload();

    // Getter und Setter
    uint8_t GetMacExtensionFieldEncoding() const;
    void SetMacExtensionFieldEncoding(uint8_t encoding);

    uint8_t GetLengthField() const;
    void SetLengthField(uint8_t length);

    IETypeFieldEncoding GetIeTypeFieldEncoding() const;
    void SetIeTypeFieldEncoding(IETypeFieldEncoding type);

        // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

  private:
    uint8_t m_macExtensionFieldEncoding; // 2 Bit
    uint8_t m_length;                    // 1 Bit
    uint8_t m_ieTypeFieldEncoding;       // 5 Bit
};

} // namespace ns3

#endif