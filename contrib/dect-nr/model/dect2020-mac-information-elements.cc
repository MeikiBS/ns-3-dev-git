#include "dect2020-mac-information-elements.h"
#include "ns3/log.h"


namespace ns3
{
NS_LOG_COMPONENT_DEFINE("Dect2020MACInformationElements");

// *******************************************************
//            DECT2020 Random Access Resource IE
//            # ETSI TS 103 636-4 V2.1.1 6.4.3.4
// *******************************************************

Dect2020RandomAccessResourceIE::Dect2020RandomAccessResourceIE()
{
}

Dect2020RandomAccessResourceIE::~Dect2020RandomAccessResourceIE()
{
}

TypeId
Dect2020RandomAccessResourceIE::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020RandomAccessResourceIE")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020RandomAccessResourceIE>();
    return tid;
}

TypeId
Dect2020RandomAccessResourceIE::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

void
Dect2020RandomAccessResourceIE::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;

    // Bit 0-2 Reserved
    byte0 |= (m_repeat & 0x03) << 3;                          // Bit 3-4
    byte0 |= (m_systemFrameNumberFieldIncluded ? 1 : 0) << 2; // Bit 5
    byte0 |= (m_channelFieldIncluded ? 1 : 0) << 1;           // Bit 6
    byte0 |= (m_separateChannelFieldIncluded ? 1 : 0) << 0;   // Bit 7

    start.WriteU8(byte0);

    if (false) // Byte1 only if subcarrier scaling factor >= 4 --> currently not implemented
    {
        // Byte 1
    }

    // Byte 2
    uint8_t byte2 = m_startSubslot & 0xFF; // Bit 0-7;

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = 0;
    byte3 |= (m_lengthType ? 1 : 0) << 7; // Bit 0
    byte3 |= (m_raraLength & 0x7F);       // Bit 1-7

    start.WriteU8(byte3);

    // Byte 4
    uint8_t byte4 = 0;
    byte4 |= (m_maxRachLengthType ? 1 : 0) << 7; // Bit 0
    byte4 |= (m_maxRachLength & 0x0F) << 3;      // Bit 1-4
    byte4 |= (m_cwMin_sig & 0x07);               // Bit 5-7

    start.WriteU8(byte4);

    // Byte 5
    uint8_t byte5 = 0;
    byte5 |= (m_dectDelay ? 1 : 0) << 7;     // Bit 0
    byte5 |= (m_responseWindow & 0x0F) << 3; // Bit 1-4
    byte5 |= (m_cwMax_sig & 0x07);           // Bit 5-7

    start.WriteU8(byte5);

    if (m_repeat == 1 || m_repeat == 2)
    {
        // Byte 6
        uint8_t byte6 = m_repitition; // Bit 0-7

        start.WriteU8(byte6);

        // Byte 7
        uint8_t byte7 = m_validity; // Bit 0-7

        start.WriteU8(byte7);
    }

    if (m_systemFrameNumberFieldIncluded)
    {
        // Byte 8
        uint8_t byte8 = m_systemFrameNumberValue; // Bit 0-7

        start.WriteU8(byte8);
    }

    if (m_channelFieldIncluded)
    {
        // Byte 9
        uint8_t byte9 = (m_channelAbsoluteCarrierCentryFrequency >> 8) & 0x1F; // Bit 3-7

        start.WriteU8(byte9);

        // Byte 10
        uint8_t byte10 = m_channelAbsoluteCarrierCentryFrequency & 0xFF; // Bit 1-7

        start.WriteU8(byte10);
    }

    if (m_separateChannelFieldIncluded)
    {
        // Byte 11
        uint8_t byte11 = (m_separateChannelAbsoluteCarrierCentryFrequency >> 8) & 0x1F; // Bit 3-7

        start.WriteU8(byte11);

        // Byte 12
        uint8_t byte12 = m_separateChannelAbsoluteCarrierCentryFrequency & 0xFF; // Bit 1-7

        start.WriteU8(byte12);
    }
}

uint32_t
Dect2020RandomAccessResourceIE::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;

    // Byte 0
    uint8_t byte0 = i.ReadU8();

    m_repeat = (byte0 >> 3) & 0x03; // Bits 3–4

    m_systemFrameNumberFieldIncluded = (byte0 & (1 << 2)) != 0; // Bit 5
    m_channelFieldIncluded = (byte0 & (1 << 1)) != 0;           // Bit 6
    m_separateChannelFieldIncluded = (byte0 & (1 << 0)) != 0;   // Bit 7

    // Byte 1 skipped (currently unused / reserved for μ ≥ 4)
    // i.Next();

    // Byte 2
    m_startSubslot = i.ReadU8();

    // Byte 3
    uint8_t byte3 = i.ReadU8();
    m_lengthType = (byte3 >> 7) & 0x01; // Bit 0
    m_raraLength = byte3 & 0x7F;        // Bits 1–7

    // Byte 4
    uint8_t byte4 = i.ReadU8();
    m_maxRachLengthType = (byte4 >> 7) & 0x01; // Bit 0
    m_maxRachLength = (byte4 >> 3) & 0x0F;     // Bits 1–4
    m_cwMin_sig = byte4 & 0x07;                // Bits 5–7

    // Byte 5
    uint8_t byte5 = i.ReadU8();
    m_dectDelay = (byte5 >> 7) & 0x01;      // Bit 0
    m_responseWindow = (byte5 >> 3) & 0x0F; // Bits 1–4
    m_cwMax_sig = byte5 & 0x07;             // Bits 5–7

    // Byte 6–7
    if (m_repeat == 1 || m_repeat == 2)
    {
        m_repitition = i.ReadU8(); // Byte 6
        m_validity = i.ReadU8();   // Byte 7
    }

    // Byte 8
    if (m_systemFrameNumberFieldIncluded)
    {
        m_systemFrameNumberValue = i.ReadU8(); // Byte 8
    }

    // Byte 9–10
    if (m_channelFieldIncluded)
    {
        uint8_t byte9 = i.ReadU8();  // Bits 3–7 relevant
        uint8_t byte10 = i.ReadU8(); // Bits 1–7

        m_channelAbsoluteCarrierCentryFrequency = ((byte9 & 0x1F) << 8) | byte10;
    }

    // Byte 11–12
    if (m_separateChannelFieldIncluded)
    {
        uint8_t byte11 = i.ReadU8(); // Bits 3–7 relevant
        uint8_t byte12 = i.ReadU8(); // Bits 1–7

        m_separateChannelAbsoluteCarrierCentryFrequency = ((byte11 & 0x1F) << 8) | byte12;
    }

    return i.GetDistanceFrom(start);
}

void
Dect2020RandomAccessResourceIE::Print(std::ostream& os) const
{
    os << "Dect2020RandomAccessResourceIE:" << std::endl;
    os << "  Repeat: " << static_cast<uint16_t>(m_repeat) << std::endl;
    os << "  System Frame Number Field Included: " << std::boolalpha
       << m_systemFrameNumberFieldIncluded << std::endl;
    os << "  Channel Field Included: " << std::boolalpha << m_channelFieldIncluded << std::endl;
    os << "  Separate Channel Field Included: " << std::boolalpha << m_separateChannelFieldIncluded
       << std::endl;

    os << "  Start Subslot: " << m_startSubslot << std::endl;
    os << "  Length Type (0=subslots, 1=slots): " << std::boolalpha << m_lengthType << std::endl;
    os << "  RARA Length: " << static_cast<uint16_t>(m_raraLength) << std::endl;

    os << "  Max RACH Length Type: " << std::boolalpha << m_maxRachLengthType << std::endl;
    os << "  Max RACH Length: " << static_cast<uint16_t>(m_maxRachLength) << std::endl;

    os << "  CWmin_sig: " << static_cast<uint16_t>(m_cwMin_sig) << std::endl;
    os << "  DECT Delay: " << std::boolalpha << m_dectDelay << std::endl;
    os << "  Response Window: " << static_cast<uint16_t>(m_responseWindow) << std::endl;
    os << "  CWmax_sig: " << static_cast<uint16_t>(m_cwMax_sig) << std::endl;

    if (m_repeat == 1 || m_repeat == 2)
    {
        os << "  Repetition: " << static_cast<uint16_t>(m_repitition) << std::endl;
        os << "  Validity: " << static_cast<uint16_t>(m_validity) << std::endl;
    }

    if (m_systemFrameNumberFieldIncluded)
    {
        os << "  System Frame Number Value: " << static_cast<uint16_t>(m_systemFrameNumberValue)
           << std::endl;
    }

    if (m_channelFieldIncluded)
    {
        os << "  Channel Absolute Carrier Center Frequency: "
           << m_channelAbsoluteCarrierCentryFrequency << std::endl;
    }

    if (m_separateChannelFieldIncluded)
    {
        os << "  Separate Channel Absolute Carrier Center Frequency: "
           << m_separateChannelAbsoluteCarrierCentryFrequency << std::endl;
    }
}

uint32_t
Dect2020RandomAccessResourceIE::GetSerializedSize() const
{
    uint32_t size = 0;

    size += 1; // Byte 0 (repeat, flags)
    // Byte 1 (reserved for μ ≥ 4) is not used

    size += 1; // Byte 2: StartSubslot (8 Bit)
    size += 1; // Byte 3: LengthType + RARA Length
    size += 1; // Byte 4: Max RACH Length Type + Max Length + CWmin
    size += 1; // Byte 5: DECT delay + ResponseWindow + CWmax

    if (m_repeat == 1 || m_repeat == 2)
    {
        size += 2; // Byte 6–7: Repetition + Validity
    }

    if (m_systemFrameNumberFieldIncluded)
    {
        size += 1; // Byte 8: SFN Value
    }

    if (m_channelFieldIncluded)
    {
        size += 2; // Bytes 9–10: Channel (13 Bit)
    }

    if (m_separateChannelFieldIncluded)
    {
        size += 2; // Bytes 11–12: Separate Channel (13 Bit)
    }

    return size;
}

void
Dect2020RandomAccessResourceIE::SetRepeat(uint8_t repeat)
{
    m_repeat = repeat & 0x03;
}

uint8_t
Dect2020RandomAccessResourceIE::GetRepeat() const
{
    return m_repeat;
}

void
Dect2020RandomAccessResourceIE::SetSystemFrameNumberFieldIncluded(bool included)
{
    m_systemFrameNumberFieldIncluded = included;
}

bool
Dect2020RandomAccessResourceIE::GetSystemFrameNumberFieldIncluded() const
{
    return m_systemFrameNumberFieldIncluded;
}

void
Dect2020RandomAccessResourceIE::SetChannelFieldIncluded(bool included)
{
    m_channelFieldIncluded = included;
}

bool
Dect2020RandomAccessResourceIE::GetChannelFieldIncluded() const
{
    return m_channelFieldIncluded;
}

void
Dect2020RandomAccessResourceIE::SetSeparateChannelFieldIncluded(bool included)
{
    m_separateChannelFieldIncluded = included;
}

bool
Dect2020RandomAccessResourceIE::GetSeparateChannelFieldIncluded() const
{
    return m_separateChannelFieldIncluded;
}

void
Dect2020RandomAccessResourceIE::SetStartSubslot(uint16_t subslot)
{
    m_startSubslot = subslot;
}

uint16_t
Dect2020RandomAccessResourceIE::GetStartSubslot() const
{
    return m_startSubslot;
}

void
Dect2020RandomAccessResourceIE::SetLengthType(bool lengthType)
{
    m_lengthType = lengthType;
}

bool
Dect2020RandomAccessResourceIE::GetLengthType() const
{
    return m_lengthType;
}

void
Dect2020RandomAccessResourceIE::SetRaraLength(uint8_t length)
{
    m_raraLength = length & 0x7F;
}

uint8_t
Dect2020RandomAccessResourceIE::GetRaraLength() const
{
    return m_raraLength;
}

void
Dect2020RandomAccessResourceIE::SetMaxRachLengthType(bool type)
{
    m_maxRachLengthType = type;
}

bool
Dect2020RandomAccessResourceIE::GetMaxRachLengthType() const
{
    return m_maxRachLengthType;
}

void
Dect2020RandomAccessResourceIE::SetMaxRachLength(uint8_t length)
{
    m_maxRachLength = length & 0x0F;
}

uint8_t
Dect2020RandomAccessResourceIE::GetMaxRachLength() const
{
    return m_maxRachLength;
}

void
Dect2020RandomAccessResourceIE::SetCwMinSig(uint8_t cwMinSig)
{
    m_cwMin_sig = cwMinSig & 0x07;
}

uint8_t
Dect2020RandomAccessResourceIE::GetCwMinSig() const
{
    return m_cwMin_sig;
}

void
Dect2020RandomAccessResourceIE::SetDectDelay(bool dectDelay)
{
    m_dectDelay = dectDelay;
}

bool
Dect2020RandomAccessResourceIE::GetDectDelay() const
{
    return m_dectDelay;
}

void
Dect2020RandomAccessResourceIE::SetResponseWindow(uint8_t responseWindow)
{
    m_responseWindow = responseWindow & 0x0F;
}

uint8_t
Dect2020RandomAccessResourceIE::GetResponseWindow() const
{
    return m_responseWindow;
}

void
Dect2020RandomAccessResourceIE::SetCwMaxSig(uint8_t cwMaxSig)
{
    m_cwMax_sig = cwMaxSig & 0x07;
}

uint8_t
Dect2020RandomAccessResourceIE::GetCwMaxSig() const
{
    return m_cwMax_sig;
}

void
Dect2020RandomAccessResourceIE::SetRepetition(uint8_t repetition)
{
    m_repitition = repetition;
}

uint8_t
Dect2020RandomAccessResourceIE::GetRepetition() const
{
    return m_repitition;
}

void
Dect2020RandomAccessResourceIE::SetValidity(uint8_t validity)
{
    m_validity = validity;
}

uint8_t
Dect2020RandomAccessResourceIE::GetValidity() const
{
    return m_validity;
}

void
Dect2020RandomAccessResourceIE::SetSystemFrameNumberValue(uint8_t sfn)
{
    m_systemFrameNumberValue = sfn;
}

uint8_t
Dect2020RandomAccessResourceIE::GetSystemFrameNumberValue() const
{
    return m_systemFrameNumberValue;
}

void
Dect2020RandomAccessResourceIE::SetChannelAbsoluteCarrierCenterFrequency(uint16_t freq)
{
    m_channelAbsoluteCarrierCentryFrequency = freq & 0x1FFF;
}

uint16_t
Dect2020RandomAccessResourceIE::GetChannelAbsoluteCarrierCenterFrequency() const
{
    return m_channelAbsoluteCarrierCentryFrequency;
}

void
Dect2020RandomAccessResourceIE::SetSeparateChannelAbsoluteCarrierCenterFrequency(uint16_t freq)
{
    m_separateChannelAbsoluteCarrierCentryFrequency = freq & 0x1FFF;
}

uint16_t
Dect2020RandomAccessResourceIE::GetSeparateChannelAbsoluteCarrierCenterFrequency() const
{
    return m_separateChannelAbsoluteCarrierCentryFrequency;
}

// *******************************************************
//            DECT2020 RD Capability IE
//            # ETSI TS 103 636-4 V2.1.1 6.4.3.5
// *******************************************************

Dect2020RdCapabilityIE::Dect2020RdCapabilityIE()
{
    m_numOfPhyCapabilities = 0;
}

Dect2020RdCapabilityIE::~Dect2020RdCapabilityIE()
{
}

TypeId
Dect2020RdCapabilityIE::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Dect2020RdCapabilityIE")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020RdCapabilityIE>();
    return tid;
}

TypeId
Dect2020RdCapabilityIE::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

uint32_t
Dect2020RdCapabilityIE::GetSerializedSize() const
{
    return 7;
}

void
Dect2020RdCapabilityIE::Print(std::ostream& os) const
{
    os << "RD Capability IE:"
       << " Number of PHY capabilities=" << static_cast<uint32_t>(m_numOfPhyCapabilities) << std::endl 
       << ", Release=" << static_cast<uint32_t>(m_Release)<< std::endl 
       << ", GroupAssign=" << m_groupAssignment<< std::endl 
       << ", Paging=" << m_paging<< std::endl 
       << ", OperatingModes=" << static_cast<uint32_t>(m_operatingModes)<< std::endl 
       << ", Mesh=" << m_mesh<< std::endl 
       << ", ScheduledAccessDataTransfer=" << m_scheduledAccessDataTransfer<< std::endl 
       << ", MACSec=" << static_cast<uint32_t>(m_macSecurity)<< std::endl 
       << ", DLC=" << static_cast<uint32_t>(m_dlcServiceType)<< std::endl 
       << ", PowerClass=" << static_cast<uint32_t>(m_rdPowerClass)<< std::endl 
       << ", NSSFoRx=" << static_cast<uint32_t>(m_maxNssFoRx)<< std::endl 
       << ", RxDiv=" << static_cast<uint32_t>(m_rxForTxDiversity)<< std::endl 
       << ", RxGain=" << static_cast<uint32_t>(m_rxGain)<< std::endl 
       << ", MaxMCS=" << static_cast<uint32_t>(m_maxMcs)<< std::endl 
       << ", Soft-Buffer=" << static_cast<uint32_t>(m_softBufferSize)<< std::endl 
       << ", HARQProc=" << static_cast<uint32_t>(m_numOfHarqProcesses)<< std::endl 
       << ", HARQDelay=" << static_cast<uint32_t>(m_harqFeedbackDelay)<< std::endl 
       << ", DDelay=" << m_dDelay<< std::endl 
       << ", HalfDULP=" << m_halfDulp << std::endl;
}


void
Dect2020RdCapabilityIE::Serialize(Buffer::Iterator start) const
{
    // Byte 0
    uint8_t byte0 = 0;
    byte0 |= (m_numOfPhyCapabilities & 0x07) << 5; // Bits 0-2
    byte0 |= (m_Release & 0x1F) << 0;              // Bits 3-7

    start.WriteU8(byte0);

    // Byte 1
    uint8_t byte1 = 0;
    // Bit 0-1 Reserved
    byte1 |= (m_groupAssignment ? 1 : 0) << 5;             // Bit 2
    byte1 |= (m_paging ? 1 : 0) << 4;                      // Bit 3
    byte1 |= (m_operatingModes & 0x03) << 2;               // Bit 4-5
    byte1 |= (m_mesh ? 1 : 0) << 1;                        // Bit 6
    byte1 |= (m_scheduledAccessDataTransfer ? 1 : 0) << 0; // Bit 7

    start.WriteU8(byte1);

    // Byte 2
    uint8_t byte2 = 0;
    byte2 |= (m_macSecurity & 0x07) << 5;    // Bit 0-2
    byte2 |= (m_dlcServiceType & 0x07) << 2; // Bit 3-5
    // Bit 6-7 Reserved

    start.WriteU8(byte2);

    // Byte 3
    uint8_t byte3 = 0;
    // Bit 0 Reserved
    byte3 |= (m_rdPowerClass & 0x07) << 4;     // Bit 1-3
    byte3 |= (m_maxNssFoRx & 0x03) << 2;       // Bit 4-5
    byte3 |= (m_rxForTxDiversity & 0x03) << 0; // Bit 6-7

    start.WriteU8(byte3);

    // Byte 4
    uint8_t byte4 = 0;
    byte4 |= (m_rxGain & 0x0F) << 4; // Bit 0-3
    byte4 |= (m_maxMcs & 0x0F) << 0; // Bit 4-7

    start.WriteU8(byte4);

    // Byte 5
    uint8_t byte5 = 0;
    byte5 |= (m_softBufferSize & 0x0F) << 4;     // Bit 0-3
    byte5 |= (m_numOfHarqProcesses & 0x03) << 2; // Bit 4-5
    // Bit 6-7 Reserved

    start.WriteU8(byte5);

    // Byte 6
    uint8_t byte6 = 0;
    byte6 |= (m_harqFeedbackDelay & 0x0F) << 4; // Bit 0-3
    byte6 |= (m_dDelay ? 1 : 0) << 3;           // Bit 4
    byte6 |= (m_halfDulp ? 1 : 0) << 2;         // Bit 5
    // Bit 6-7 Reserved

    start.WriteU8(byte6);
}

uint32_t
Dect2020RdCapabilityIE::Deserialize(Buffer::Iterator start)
{
    // Byte 0
    uint8_t byte0 = start.ReadU8();

    m_numOfPhyCapabilities = (byte0 >> 5) & 0x07; // Bit 0-2
    m_Release = byte0 & 0x1F;                     // Bit 3-7

    // Byte 1
    uint8_t byte1 = start.ReadU8();

    m_groupAssignment = ((byte1 >> 5) & 0x01);
    m_paging = ((byte1 >> 4) & 0x01);
    m_operatingModes = (byte1 >> 2) & 0x03;
    m_mesh = ((byte1 >> 1) & 0x01);
    m_scheduledAccessDataTransfer = (byte1 & 0x01);

    // Byte 2
    uint8_t byte2 = start.ReadU8();

    m_macSecurity = (byte2 >> 5) & 0x07;
    m_dlcServiceType = (byte2 >> 2) & 0x07;
    // Bit 6-7 reserved

    // Byte 3
    uint8_t byte3 = start.ReadU8();

    m_rdPowerClass = (byte3 >> 4) & 0x07;
    m_maxNssFoRx = (byte3 >> 2) & 0x03;
    m_rxForTxDiversity = byte3 & 0x03;

    // Byte 4
    uint8_t byte4 = start.ReadU8();

    m_rxGain = (byte4 >> 4) & 0x0F;
    m_maxMcs = byte4 & 0x0F;

    // Byte 5
    uint8_t byte5 = start.ReadU8();

    m_softBufferSize = (byte5 >> 4) & 0x0F;
    m_numOfHarqProcesses = (byte5 >> 2) & 0x03;
    // Bit 6–7 reserved

    // Byte 6
    uint8_t byte6 = start.ReadU8();

    m_harqFeedbackDelay = (byte6 >> 4) & 0x0F;
    m_dDelay = ((byte6 >> 3) & 0x01);
    m_halfDulp = ((byte6 >> 2) & 0x01);
    // Bit 6–7 reserved

    return GetSerializedSize();
}

void
Dect2020RdCapabilityIE::SetNumOfPhyCapabilities(uint8_t val)
{
    m_numOfPhyCapabilities = val & 0x07;
}

uint8_t
Dect2020RdCapabilityIE::GetNumOfPhyCapabilities() const
{
    return m_numOfPhyCapabilities;
}

void
Dect2020RdCapabilityIE::SetRelease(uint8_t val)
{
    m_Release = val & 0x1F;
}

uint8_t
Dect2020RdCapabilityIE::GetRelease() const
{
    return m_Release;
}

void
Dect2020RdCapabilityIE::SetGroupAssignment(bool enabled)
{
    m_groupAssignment = enabled;
}

bool
Dect2020RdCapabilityIE::GetGroupAssignment() const
{
    return m_groupAssignment;
}

void
Dect2020RdCapabilityIE::SetPaging(bool enabled)
{
    m_paging = enabled;
}

bool
Dect2020RdCapabilityIE::GetPaging() const
{
    return m_paging;
}

void
Dect2020RdCapabilityIE::SetOperatingModes(uint8_t val)
{
    m_operatingModes = val & 0x03;
}

uint8_t
Dect2020RdCapabilityIE::GetOperatingModes() const
{
    return m_operatingModes;
}

void
Dect2020RdCapabilityIE::SetMesh(bool enabled)
{
    m_mesh = enabled;
}

bool
Dect2020RdCapabilityIE::GetMesh() const
{
    return m_mesh;
}

void
Dect2020RdCapabilityIE::SetScheduledAccessDataTransfer(bool enabled)
{
    m_scheduledAccessDataTransfer = enabled;
}

bool
Dect2020RdCapabilityIE::GetScheduledAccessDataTransfer() const
{
    return m_scheduledAccessDataTransfer;
}

void
Dect2020RdCapabilityIE::SetMacSecurity(uint8_t val)
{
    m_macSecurity = val & 0x07;
}

uint8_t
Dect2020RdCapabilityIE::GetMacSecurity() const
{
    return m_macSecurity;
}

void
Dect2020RdCapabilityIE::SetDlcServiceType(uint8_t val)
{
    m_dlcServiceType = val & 0x07;
}

uint8_t
Dect2020RdCapabilityIE::GetDlcServiceType() const
{
    return m_dlcServiceType;
}

void
Dect2020RdCapabilityIE::SetRdPowerClass(uint8_t val)
{
    m_rdPowerClass = val & 0x07;
}

uint8_t
Dect2020RdCapabilityIE::GetRdPowerClass() const
{
    return m_rdPowerClass;
}

void
Dect2020RdCapabilityIE::SetMaxNssFoRx(uint8_t val)
{
    m_maxNssFoRx = val & 0x07;
}

uint8_t
Dect2020RdCapabilityIE::GetMaxNssFoRx() const
{
    return m_maxNssFoRx;
}

void
Dect2020RdCapabilityIE::SetRxForTxDiversity(uint8_t val)
{
    m_rxForTxDiversity = val & 0x07;
}

uint8_t
Dect2020RdCapabilityIE::GetRxForTxDiversity() const
{
    return m_rxForTxDiversity;
}

void
Dect2020RdCapabilityIE::SetRxGain(uint8_t val)
{
    m_rxGain = val & 0x0F;
}

uint8_t
Dect2020RdCapabilityIE::GetRxGain() const
{
    return m_rxGain;
}

void
Dect2020RdCapabilityIE::SetMaxMcs(uint8_t val)
{
    m_maxMcs = val & 0x0F;
}

uint8_t
Dect2020RdCapabilityIE::GetMaxMcs() const
{
    return m_maxMcs;
}

void
Dect2020RdCapabilityIE::SetSoftBufferSize(uint8_t val)
{
    m_softBufferSize = val & 0x0F;
}

uint8_t
Dect2020RdCapabilityIE::GetSoftBufferSize() const
{
    return m_softBufferSize;
}

void
Dect2020RdCapabilityIE::SetNumOfHarqProcesses(uint8_t val)
{
    m_numOfHarqProcesses = val & 0x03;
}

uint8_t
Dect2020RdCapabilityIE::GetNumOfHarqProcesses() const
{
    return m_numOfHarqProcesses;
}

void
Dect2020RdCapabilityIE::SetHarqFeedbackDelay(uint8_t val)
{
    m_harqFeedbackDelay = val & 0x0F;
}

uint8_t
Dect2020RdCapabilityIE::GetHarqFeedbackDelay() const
{
    return m_harqFeedbackDelay;
}

void
Dect2020RdCapabilityIE::SetDDelay(bool enabled)
{
    m_dDelay = enabled;
}

bool
Dect2020RdCapabilityIE::GetDDelay() const
{
    return m_dDelay;
}

void
Dect2020RdCapabilityIE::SetHalfDulp(bool enabled)
{
    m_halfDulp = enabled;
}

bool
Dect2020RdCapabilityIE::GetHalfDulp() const
{
    return m_halfDulp;
}

// *******************************************************
//            DECT2020 Association Control IE
//            # ETSI TS 103 636-4 V2.1.1 6.4.3.18
// *******************************************************

Dect2020AssociationControlIE::Dect2020AssociationControlIE()
    : m_cbM(false),
      m_dlDataReception(0),
      m_ulPeriod(0)
{
}

Dect2020AssociationControlIE::~Dect2020AssociationControlIE()
{
}

TypeId
Dect2020AssociationControlIE::GetTypeId()
{
    static TypeId tid = TypeId("ns3::Dect2020AssociationControlIE")
                            .SetParent<Header>()
                            .SetGroupName("Dect2020")
                            .AddConstructor<Dect2020AssociationControlIE>();
    return tid;
}

TypeId
Dect2020AssociationControlIE::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
Dect2020AssociationControlIE::Print(std::ostream& os) const
{
    os << "CB_M = " << (m_cbM ? 1 : 0)
       << ", DLdataReception = " << static_cast<uint32_t>(m_dlDataReception)
       << ", ULPeriod = " << static_cast<uint32_t>(m_ulPeriod);
}

uint32_t
Dect2020AssociationControlIE::GetSerializedSize() const
{
    return 1; // 1 Byte
}

void
Dect2020AssociationControlIE::Serialize(Buffer::Iterator start) const
{
    uint8_t byte = 0;
    if (m_cbM)
    {
        byte |= (1 << 7); // Bit 0
    }
    byte |= (m_dlDataReception & 0x07) << 4; // Bits 1-3
    byte |= (m_ulPeriod & 0x0F);             // Bits 4–7

    start.WriteU8(byte);
}

uint32_t
Dect2020AssociationControlIE::Deserialize(Buffer::Iterator start)
{
    uint8_t byte = start.ReadU8();

    m_cbM = (byte & 0x80) != 0;
    m_dlDataReception = (byte >> 4) & 0x07;
    m_ulPeriod = byte & 0x0F;

    return 1;
}

void
Dect2020AssociationControlIE::SetClusterBeaconMonitoring(bool enable)
{
    m_cbM = enable;
}

bool
Dect2020AssociationControlIE::GetClusterBeaconMonitoring() const
{
    return m_cbM;
}

void
Dect2020AssociationControlIE::SetDlDataReception(uint8_t code)
{
    m_dlDataReception = code & 0x07;
}

uint8_t
Dect2020AssociationControlIE::GetDlDataReception() const
{
    return m_dlDataReception;
}

void
Dect2020AssociationControlIE::SetUlPeriod(uint8_t code)
{
    m_ulPeriod = code & 0x0F;
}

uint8_t
Dect2020AssociationControlIE::GetUlPeriod() const
{
    return m_ulPeriod;
}

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

// Dect2020ResourceAllocationIE::Dect2020ResourceAllocationIE()
// {
// }

// Dect2020ResourceAllocationIE::~Dect2020ResourceAllocationIE()
// {
// }

// TypeId
// Dect2020ResourceAllocationIE::GetTypeId(void)
// {
//     static TypeId tid = TypeId("ns3::Dect2020ResourceAllocationIE")
//                             .SetParent<Header>()
//                             .SetGroupName("Dect2020")
//                             .AddConstructor<Dect2020ResourceAllocationIE>();
//     return tid;
// }

// TypeId
// Dect2020ResourceAllocationIE::GetInstanceTypeId(void) const
// {
//     return GetTypeId();
// }

// void
// Dect2020ResourceAllocationIE::Serialize(Buffer::Iterator start) const
// {
//     // Byte 0
//     uint8_t byte0 = 0;
//     byte0 |= (m_allocationType & 0x03) << 6;             // Bits 0-1
//     byte0 |= (m_add ? 1 : 0) << 5;                       // Bit 2
//     byte0 |= (m_iD ? 1 : 0) << 4;                        // Bit 3
//     byte0 |= (m_repeat & 0x07) << 1;                     // Bits 4-6
//     byte0 |= (m_systemFrameNumberFieldIncluded ? 1 : 0); // Bit 7
//     start.WriteU8(byte0);

//     // when the Allocation Type is set to 00 - release all scheduled resources
//     if ((m_allocationType & 0x03) == 0b00)
//     {
//         return;
//     }

//     // Byte 1
//     uint8_t byte1 = 0;
//     byte1 |= (m_channelFieldIncluded ? 1 : 0) << 7;
//     byte1 |= (m_rlf ? 1 : 0) << 6;
//     // Bit 2-7 reserved
//     start.WriteU8(byte1);

//     if(m_allocationType == 1 || m_allocationType == 3)
//     {

//     }

// }

// void
// Dect2020ResourceAllocationIE::SetAllocationType(uint8_t allocationType)
// {
//     m_allocationType = allocationType & 0x03; // 2 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetAllocationType() const
// {
//     return m_allocationType;
// }

// void
// Dect2020ResourceAllocationIE::SetAdd(bool add)
// {
//     m_add = add;
// }

// bool
// Dect2020ResourceAllocationIE::GetAdd() const
// {
//     return m_add;
// }

// void
// Dect2020ResourceAllocationIE::SetID(bool iD)
// {
//     m_iD = iD;
// }

// bool
// Dect2020ResourceAllocationIE::GetID() const
// {
//     return m_iD;
// }

// void
// Dect2020ResourceAllocationIE::SetRepeat(uint8_t repeat)
// {
//     m_repeat = repeat & 0x07; // 3 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetRepeat() const
// {
//     return m_repeat;
// }

// void
// Dect2020ResourceAllocationIE::SetSystemFrameNumberFieldIncluded(bool systemFrameNumber)
// {
//     m_systemFrameNumberFieldIncluded = systemFrameNumber;
// }

// bool
// Dect2020ResourceAllocationIE::GetSystemFrameNumberFieldIncluded() const
// {
//     return m_systemFrameNumberFieldIncluded;
// }

// void
// Dect2020ResourceAllocationIE::SetChannelFieldIncluded(bool channelFieldIncluded)
// {
//     m_channelFieldIncluded = channelFieldIncluded;
// }

// bool
// Dect2020ResourceAllocationIE::GetChannelFieldIncluded() const
// {
//     return m_channelFieldIncluded;
// }

// void
// Dect2020ResourceAllocationIE::SetRLF(bool rlf)
// {
//     m_rlf = rlf;
// }

// bool
// Dect2020ResourceAllocationIE::GetRLF() const
// {
//     return m_rlf;
// }

// void
// Dect2020ResourceAllocationIE::SetStartSubslotDownlink(uint16_t startSubslotDownlink)
// {
//     m_startSubslotDownlink = startSubslotDownlink & 0xFF; // 8 Bit
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetStartSubslotDownlink() const
// {
//     return m_startSubslotDownlink;
// }

// void
// Dect2020ResourceAllocationIE::SetStartSubslotUplink(uint16_t startSubslotUplink)
// {
//     m_startSubslotUplink = startSubslotUplink & 0xFF; // 8 Bit
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetStartSubslotUplink() const
// {
//     return m_startSubslotUplink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthTypeDownlink(bool lengthTypeDownlink)
// {
//     m_lengthTypeDownlink = lengthTypeDownlink;
// }

// bool
// Dect2020ResourceAllocationIE::GetLengthTypeDownlink() const
// {
//     return m_lengthTypeDownlink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthTypeUplink(bool lengthTypeUplink)
// {
//     m_lengthTypeUplink = lengthTypeUplink;
// }

// bool
// Dect2020ResourceAllocationIE::GetLengthTypeUplink() const
// {
//     return m_lengthTypeUplink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthDownlink(uint8_t lengthDownlink)
// {
//     m_lengthDownlink = lengthDownlink & 0x7F; // 7 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetLengthDownlink() const
// {
//     return m_lengthDownlink;
// }

// void
// Dect2020ResourceAllocationIE::SetLengthUplink(uint8_t lengthUplink)
// {
//     m_lengthUplink = lengthUplink & 0x7F; // 7 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetLengthUplink() const
// {
//     return m_lengthUplink;
// }

// void
// Dect2020ResourceAllocationIE::SetShortRdId(uint16_t shortRdId)
// {
//     m_shortRdId = shortRdId;
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetShortRdId() const
// {
//     return m_shortRdId;
// }

// void
// Dect2020ResourceAllocationIE::SetRepetition(uint8_t repetition)
// {
//     m_repitition = repetition;
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetRepetition() const
// {
//     return m_repitition;
// }

// void
// Dect2020ResourceAllocationIE::SetValidity(uint8_t validity)
// {
//     m_validity = validity;
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetValidity() const
// {
//     return m_validity;
// }

// void
// Dect2020ResourceAllocationIE::SetSFNValue(uint8_t sfnValue)
// {
//     n_sfnValue = sfnValue;
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetSFNValue() const
// {
//     return n_sfnValue;
// }

// void
// Dect2020ResourceAllocationIE::SetChannel(uint16_t channel)
// {
//     m_channel = channel & 0x1FFF; // 13 Bit
// }

// uint16_t
// Dect2020ResourceAllocationIE::GetChannel() const
// {
//     return m_channel;
// }

// void
// Dect2020ResourceAllocationIE::SetDectScheduledResourceFailure(uint8_t failure)
// {
//     m_dectScheduledResourceFailure = failure & 0x0F; // 4 Bit
// }

// uint8_t
// Dect2020ResourceAllocationIE::GetDectScheduledResourceFailure() const
// {
//     return m_dectScheduledResourceFailure;
// }

} // namespace ns3