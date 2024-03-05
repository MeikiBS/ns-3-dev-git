#include "end-device-dect-nr-mac.h"

namespace ns3
{

namespace dect2020
{

NS_OBJECT_ENSURE_REGISTERED(EndDeviceDect2020Mac);

TypeId
EndDeviceDect2020Mac::GetTypeId()
{
    static TypeId tid = TypeId("ns3::dect2020::EndDeviceDect2020Mac")
                        .SetParent<DectMac>();
    return tid;
}

EndDeviceDect2020Mac::EndDeviceDect2020Mac()
{
}

EndDeviceDect2020Mac::~EndDeviceDect2020Mac()
{
}

}   // namespace dect2020

}

