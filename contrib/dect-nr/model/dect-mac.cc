#include "dect-mac.h"

namespace ns3
{
namespace dect2020
{

NS_OBJECT_ENSURE_REGISTERED(DectMac);

TypeId
DectMac::GetTypeId()
{
    static TypeId tid = TypeId("ns3::dect2020::DectMac").SetParent<Object>();
    return tid;
}

DectMac::DectMac()
{
    m_macAddress = Mac48Address::Allocate();
}

DectMac::~DectMac()
{
}

} // namespace dect2020
} // namespace ns3
