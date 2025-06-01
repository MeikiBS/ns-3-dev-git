#ifndef DECT2020_PHYSICAL_HEADER_FIELD
#define DECT2020_PHYSICAL_HEADER_FIELD

#include "ns3/address.h"
#include "ns3/header.h"
#include "ns3/nstime.h"

/**
 * @file dect2020-physical-header-field.h
 * @ingroup dect2020
 * @brief Definition of physical layer control field headers (Type 1 and Type 2) for DECT-2020 NR.
 *
 * This header defines two classes that represent the physical layer control fields
 * used in DECT-2020 NR, based on ETSI TS 103 636-4 Section 6.2.1:
 * - Dect2020PHYControlFieldType1: for basic one-way transmissions (Format 000)
 * - Dect2020PHYControlFieldType2: for enhanced transmissions with feedback and spatial streams
 * (Format 001)
 *
 * These headers are serialized and prepended to PHY layer packets for transmission via
 * SpectrumChannel.
 */

namespace ns3
{

// *******************************************************
//      DECT2020 Physical Layer Control Field: Type 1
//            # ETSI TS 103 636-4 V2.1.1 Table 6.2.1-1
// *******************************************************

/**
 * @class Dect2020PHYControlFieldType1
 * @brief Represents the PHY Control Field Type 1 (Format 000) for DECT-2020 NR.
 *
 * This header includes minimal PHY control information such as:
 * - Packet length (slots or subslots)
 * - Short network ID (last 8 bits of full Network ID)
 * - Transmitter identity
 * - Transmit power
 * - MCS (Modulation and Coding Scheme)
 *
 * Based on ETSI TS 103 636-4 Table 6.2.1-1.
 *
 * This control field is used in basic unicast or broadcast transmissions where
 * feedback or spatial stream details are not required.
 */

class Dect2020PHYControlFieldType1 : public Header
{
  public:
    Dect2020PHYControlFieldType1();
    Dect2020PHYControlFieldType1(uint8_t packetLengthType,
                                 uint8_t packetLength,
                                 uint32_t longNetworkID,
                                 uint16_t transmitterIdentity,
                                 uint8_t transmitPower,
                                 uint8_t dFMCS);
    virtual ~Dect2020PHYControlFieldType1();

    // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

    // Setter und Getter
    void SetPacketLengthType(uint8_t packetLengthType);
    uint8_t GetPacketLengthType();
    void SetPacketLength(uint8_t packetLength);
    uint8_t GetPacketLength();
    void SetShortNetworkID(uint32_t longNetworkID);
    uint8_t GetShortNetworkID();
    void SetTransmitterIdentity(uint16_t transmitterIdentity);
    uint16_t GetTransmitterIdentity();
    void SetTransmitPower(uint8_t transmitPower);
    uint8_t GetTransmitPower();
    void SetDFMCS(uint8_t dFMCS);
    uint8_t GetDFMCS();

  private:
    uint8_t m_headerFormat;
    uint8_t m_packetLengthType;
    uint8_t m_packetLength;
    uint8_t m_shortNetworkID;
    uint16_t m_transmitterIdentity;
    uint8_t m_transmitPower;
    uint8_t m_dFMCS;
    uint32_t m_networkId;          // 24 Bits relevant
    uint32_t m_transmitterAddress; // 32 Bits
};

// *******************************************************
// DECT2020 Physical Layer Control Field: Type 2, Format 001
//            # ETSI TS 103 636-4 V2.1.1 6.2.1-2a
// *******************************************************

/**
 * @class Dect2020PHYControlFieldType2
 * @brief Represents the PHY Control Field Type 2 (Format 001) for DECT-2020 NR.
 *
 * This extended header includes:
 * - All fields from Type 1 (e.g., packet length, transmitter ID, power)
 * - Receiver identity
 * - Number of spatial streams
 * - Feedback format and content (e.g., CQI, HARQ)
 *
 * Based on ETSI TS 103 636-4 V2.1.1, Table 6.2.1-2a.
 */


class Dect2020PHYControlFieldType2 : public Header
{
  public:
    Dect2020PHYControlFieldType2();
    virtual ~Dect2020PHYControlFieldType2();

    // Überladene Methoden von Header
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    virtual void Print(std::ostream& os) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);

    // Setter und Getter
    void SetPacketLengthType(uint8_t packetLengthType);
    uint8_t GetPacketLengthType();

    void SetPacketLength(uint8_t packetLength);
    uint8_t GetPacketLength();

    void SetShortNetworkID(uint8_t longNetworkID);
    uint8_t GetShortNetworkID();

    void SetTransmitterIdentity(uint16_t transmitterIdentity);
    uint16_t GetTransmitterIdentity();

    void SetTransmitPower(uint8_t transmitPower);
    uint8_t GetTransmitPower();

    void SetDFMCS(uint8_t dFMCS);
    uint8_t GetDFMCS();

    void SetReceiverIdentity(uint16_t receiverIdentity);
    uint16_t GetReceiverIdentity() const;

    void SetNumberOfSpatialStreams(uint8_t numberOfSpatialStreams);
    uint8_t GetNumberOfSpatialStreams() const;

    void SetFeedbackFormat(uint8_t feedbackFormat);
    uint8_t GetFeedbackFormat() const;

    void SetFeedbackInfo(uint16_t feedbackInfo);
    uint16_t GetFeedbackInfo() const;

  private:
    uint8_t m_headerFormat;           // 3 Bit
    uint8_t m_packetLengthType;       // 1 Bit
    uint8_t m_packetLength;           // 4 Bit
    uint8_t m_shortNetworkID;         // 8 Bit
    uint16_t m_transmitterIdentity;   // 16 Bit
    uint8_t m_transmitPower;          // 4 Bit
    uint8_t m_dFMCS;                  // 4 Bit
    uint16_t m_receiverIdentity;      // 16 Bit
    uint8_t m_numberOfSpatialStreams; // 2 Bit

    uint8_t m_feedbackFormat; // 4 Bit
    uint16_t m_feedbackInfo;  // 12 Bit
};

} // namespace ns3

#endif // DECT2020_PHYSICAL_HEADER_FIELD
