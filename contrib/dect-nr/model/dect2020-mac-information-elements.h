#ifndef DECT2020_MAC_INFORMATION_ELEMENTS_H
#define DECT2020_MAC_INFORMATION_ELEMENTS_H

#include "ns3/header.h"

#include <cstdint>

namespace ns3
{

// *******************************************************
//            DECT2020 Random Access Resource IE
//            # ETSI TS 103 636-4 V2.1.1 6.4.3.4
// *******************************************************

class Dect2020RandomAccessResourceIE : public Header
{
  public:
    Dect2020RandomAccessResourceIE();
    virtual ~Dect2020RandomAccessResourceIE();

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

    // Getter and Setter
    // Getter and Setter
    void SetRepeat(uint8_t repeat);
    uint8_t GetRepeat() const;

    void SetSystemFrameNumberFieldIncluded(bool included);
    bool GetSystemFrameNumberFieldIncluded() const;

    void SetChannelFieldIncluded(bool included);
    bool GetChannelFieldIncluded() const;

    void SetSeparateChannelFieldIncluded(bool included);
    bool GetSeparateChannelFieldIncluded() const;

    void SetStartSubslot(uint16_t subslot);
    uint16_t GetStartSubslot() const;

    void SetLengthType(bool lengthType);
    bool GetLengthType() const;

    void SetRaraLength(uint8_t length);
    uint8_t GetRaraLength() const;

    void SetMaxRachLengthType(bool type);
    bool GetMaxRachLengthType() const;

    void SetMaxRachLength(uint8_t length);
    uint8_t GetMaxRachLength() const;

    void SetCwMinSig(uint8_t cwMinSig);
    uint8_t GetCwMinSig() const;

    void SetDectDelay(bool dectDelay);
    bool GetDectDelay() const;

    void SetResponseWindow(uint8_t responseWindow);
    uint8_t GetResponseWindow() const;

    void SetCwMaxSig(uint8_t cwMaxSig);
    uint8_t GetCwMaxSig() const;

    void SetRepetition(uint8_t repetition);
    uint8_t GetRepetition() const;

    void SetValidity(uint8_t validity);
    uint8_t GetValidity() const;

    void SetSystemFrameNumberValue(uint8_t sfn);
    uint8_t GetSystemFrameNumberValue() const;

    void SetChannelAbsoluteCarrierCenterFrequency(uint16_t freq);
    uint16_t GetChannelAbsoluteCarrierCenterFrequency() const;

    void SetSeparateChannelAbsoluteCarrierCenterFrequency(uint16_t freq);
    uint16_t GetSeparateChannelAbsoluteCarrierCenterFrequency() const;

  private:
    uint8_t m_repeat;                      // 2 Bit
    bool m_systemFrameNumberFieldIncluded; // 1 Bit --> SFN
    bool m_channelFieldIncluded;           // 1 Bit
    bool m_separateChannelFieldIncluded;   // 1 Bit
    uint16_t m_startSubslot;               // 8 Bit. For subcarrier scaling factor <= 4,
    //                                    // otherwise 9 Bit. Note: Only the 8 Bit variant is
    //                                    currently implemented
    bool m_lengthType;    // 1 Bit. 0 = length in subslots, 1 = length in slots
    uint8_t m_raraLength; // 7 Bit. The length of the random access resource allocation in subslots
                          // or slots
    bool m_maxRachLengthType;         // 1 Bit. 0 = length in subslots, 1 = length in slots
    uint8_t m_maxRachLength;          // 4 Bit
    uint8_t m_cwMin_sig;              // 3 Bit
    bool m_dectDelay;                 // 1 Bit
    uint8_t m_responseWindow;         // 4 Bit
    uint8_t m_cwMax_sig;              // 3 Bit
    uint8_t m_repitition;             // 8 Bit
    uint8_t m_validity;               // 8 Bit
    uint8_t m_systemFrameNumberValue; // 8 Bit
    uint16_t m_channelAbsoluteCarrierCentryFrequency;         // 13 Bit
    uint16_t m_separateChannelAbsoluteCarrierCentryFrequency; // 13 Bit
};

// *******************************************************
//            DECT2020 RD Capability IE
//            # ETSI TS 103 636-4 V2.1.1 6.4.3.5
// *******************************************************

// NOTE: Only "Number of PHY Capabilities = 000" is supported.
// This means that only the first 7 bytes are implemented.
// The remaining 5 octets defined in the standard are not currently used.

class Dect2020RdCapabilityIE : public Header
{
  public:
    Dect2020RdCapabilityIE();
    virtual ~Dect2020RdCapabilityIE();

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

    // Getter und Setter
        // Getter und Setter

    void SetNumOfPhyCapabilities(uint8_t val);
    uint8_t GetNumOfPhyCapabilities() const;

    void SetRelease(uint8_t val);
    uint8_t GetRelease() const;

    void SetGroupAssignment(bool enabled);
    bool GetGroupAssignment() const;

    void SetPaging(bool enabled);
    bool GetPaging() const;

    void SetOperatingModes(uint8_t val);
    uint8_t GetOperatingModes() const;

    void SetMesh(bool enabled);
    bool GetMesh() const;

    void SetScheduledAccessDataTransfer(bool enabled);
    bool GetScheduledAccessDataTransfer() const;

    void SetMacSecurity(uint8_t val);
    uint8_t GetMacSecurity() const;

    void SetDlcServiceType(uint8_t val);
    uint8_t GetDlcServiceType() const;

    void SetRdPowerClass(uint8_t val);
    uint8_t GetRdPowerClass() const;

    void SetMaxNssFoRx(uint8_t val);
    uint8_t GetMaxNssFoRx() const;

    void SetRxForTxDiversity(uint8_t val);
    uint8_t GetRxForTxDiversity() const;

    void SetRxGain(uint8_t val);
    uint8_t GetRxGain() const;

    void SetMaxMcs(uint8_t val);
    uint8_t GetMaxMcs() const;

    void SetSoftBufferSize(uint8_t val);
    uint8_t GetSoftBufferSize() const;

    void SetNumOfHarqProcesses(uint8_t val);
    uint8_t GetNumOfHarqProcesses() const;

    void SetHarqFeedbackDelay(uint8_t val);
    uint8_t GetHarqFeedbackDelay() const;

    void SetDDelay(bool enabled);
    bool GetDDelay() const;

    void SetHalfDulp(bool enabled);
    bool GetHalfDulp() const;


  private:
    uint8_t m_numOfPhyCapabilities; // 3 Bit
    uint8_t m_Release; // 5 Bit
    bool m_groupAssignment; // 1 Bit
    bool m_paging; // 1 Bit
    uint8_t m_operatingModes; // 2 Bit
    bool m_mesh; // 1 Bit 
    bool m_scheduledAccessDataTransfer; // 1 Bit
    uint8_t m_macSecurity; // 3 Bit
    uint8_t m_dlcServiceType; // 3 Bit
    uint8_t m_rdPowerClass; // 3 Bit
    uint8_t m_maxNssFoRx; // 3 Bit
    uint8_t m_rxForTxDiversity; // 3 Bit
    uint8_t m_rxGain; // 4 Bit
    uint8_t m_maxMcs; // 4 Bit
    uint8_t m_softBufferSize; // 4 Bit
    uint8_t m_numOfHarqProcesses; // 2 Bit
    uint8_t m_harqFeedbackDelay; // 4 Bit
    bool m_dDelay; // 1 Bit
    bool m_halfDulp; // 1 Bit

    // The following 5 Oktets are not used (and not implemented)in the current implementation
};

// *******************************************************
//            DECT2020 Association Control IE
//            # ETSI TS 103 636-4 V2.1.1 6.4.3.18
// *******************************************************

class Dect2020AssociationControlIE : public Header
{
  public:
    Dect2020AssociationControlIE();
    virtual ~Dect2020AssociationControlIE();

    // Überladene Methoden vom Header
    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;
    virtual uint32_t GetSerializedSize() const;
    virtual void Serialize(Buffer::Iterator start) const;
    virtual uint32_t Deserialize(Buffer::Iterator start);
    virtual void Print(std::ostream& os) const;

    // Getter / Setter

    // 0: The associated RD on maintains cluster beacon message reception.
    // 1: The associated RD on does not maintains cluster beacon message reception
    void SetClusterBeaconMonitoring(bool enable);
    bool GetClusterBeaconMonitoring() const;

    void SetDlDataReception(uint8_t code);
    uint8_t GetDlDataReception() const;

    void SetUlPeriod(uint8_t code);
    uint8_t GetUlPeriod() const;

  private:
    bool m_cbM;                // 1 Bit. Cluster Beacon Message Reception
    uint8_t m_dlDataReception; // 3 Bit
    uint8_t m_ulPeriod;        // 4 Bit
};

// *******************************************************
//            DECT2020 Resource Allocation IE
// *******************************************************

// ----------------------------------------------------------------------------------
// [Developer Note]
// This section was initially intended to implement the Resource Allocation IE
// as specified in section 6.4.3.3 (Dect2020ResourceAllocationIE).
//
// During the implementation process, it became clear that this was not the correct
// Information Element required in this context. Instead, the Random Access Resource IE
// (section 6.4.3.2) should be used.
//
// The current structure, fields, and methods in this class relate to the wrong IE
// and have therefore not been completed.
//
// Please restructure or remove this code as needed once the correct IE is implemented.
// Next step would be the Serialize Method.
// ----------------------------------------------------------------------------------
//
// [Meikel Kersting, May 2025]

// class Dect2020ResourceAllocationIE : public Header
// {
//   public:
//     Dect2020ResourceAllocationIE();
//     virtual ~Dect2020ResourceAllocationIE();

//     // Setter und Getter
//     void SetAllocationType(uint8_t allocationType);
//     uint8_t GetAllocationType() const;

//     void SetAdd(bool add);
//     bool GetAdd() const;

//     void SetID(bool iD);
//     bool GetID() const;

//     void SetRepeat(uint8_t repeat);
//     uint8_t GetRepeat() const;

//     void SetSystemFrameNumberFieldIncluded(bool systemFrameNumber);
//     bool GetSystemFrameNumberFieldIncluded() const;

//     void SetChannelFieldIncluded(bool channelFieldIncluded);
//     bool GetChannelFieldIncluded() const;

//     void SetRLF(bool rlf);
//     bool GetRLF() const;

//     void SetStartSubslotDownlink(uint16_t startSubslotDownlink);
//     uint16_t GetStartSubslotDownlink() const;

//     void SetStartSubslotUplink(uint16_t startSubslotUplink);
//     uint16_t GetStartSubslotUplink() const;

//     void SetLengthTypeDownlink(bool lengthTypeDownlink);
//     bool GetLengthTypeDownlink() const;

//     void SetLengthTypeUplink(bool lengthTypeUplink);
//     bool GetLengthTypeUplink() const;

//     void SetLengthDownlink(uint8_t lengthDownlink);
//     uint8_t GetLengthDownlink() const;

//     void SetLengthUplink(uint8_t lengthUplink);
//     uint8_t GetLengthUplink() const;

//     void SetShortRdId(uint16_t shortRdId);
//     uint16_t GetShortRdId() const;

//     void SetRepetition(uint8_t repetition);
//     uint8_t GetRepetition() const;

//     void SetValidity(uint8_t validity);
//     uint8_t GetValidity() const;

//     void SetSFNValue(uint8_t sfnValue);
//     uint8_t GetSFNValue() const;

//     void SetChannel(uint16_t channel);
//     uint16_t GetChannel() const;

//     void SetDectScheduledResourceFailure(uint8_t dectScheduledResourceFailure);
//     uint8_t GetDectScheduledResourceFailure() const;

//     // Überladene Methoden vom Header
//     static TypeId GetTypeId();
//     virtual TypeId GetInstanceTypeId() const override;
//     virtual uint32_t GetSerializedSize() const;
//     virtual void Serialize(Buffer::Iterator start) const;
//     virtual uint32_t Deserialize(Buffer::Iterator start);
//     virtual void Print(std::ostream& os) const;

//   private:
//     uint8_t m_allocationType;              // 2 Bit
//     bool m_add;                            // 1 Bit
//     bool m_iD;                             // 1 Bit
//     uint8_t m_repeat;                      // 3 Bit
//     bool m_systemFrameNumberFieldIncluded; // 1 Bit --> SFN
//     bool m_channelFieldIncluded;           // 1 Bit
//     bool m_rlf;                            // 1 Bit
//     uint16_t
//         m_startSubslotDownlink;    // 8 Bit. Downlink Variable. For subcarrier scaling factor <=
//         4,
//                                    // otherwise 9 Bit. Note: Only the 8 Bit variant is
//                                    implemented
//     uint16_t m_startSubslotUplink; // 8 Bit. Uplink Variable. for subcarrier scaling factor <= 4,
//                                    // otherwise 9 Bit. Note: Only the 8 Bit variant is
//                                    implemented
//     bool m_lengthTypeDownlink; // 1 Bit. Downlink Variable. 0 = length in subslots, 1 = length in
//                                // slots
//     bool m_lengthTypeUplink; // 1 Bit. Uplink Variable. 0 = length in subslots, 1 = length in
//     slots

//     uint8_t m_lengthDownlink; // 7 Bit. Downlink Variable. The length of the resource allocation
//     in
//                               // subslots/slots (depending on the length type field)
//     uint8_t m_lengthUplink;   // 7 Bit. Uplink Variable. The length of the resource allocation in
//                               // subslots/slots (depending on the length type field)
//     uint16_t m_shortRdId;     // 16 Bit
//     uint8_t m_repitition;     // 8 Bit
//     uint8_t m_validity;       // 8 Bit
//     uint8_t n_sfnValue;       // 8 Bit
//     uint16_t m_channel;       // 13 Bit
//     uint8_t m_dectScheduledResourceFailure; // 4 Bit
// };

} // namespace ns3

#endif