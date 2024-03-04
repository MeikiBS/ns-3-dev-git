#include "end-device-dect-nr-mac.h"

namespace ns3
{

namespace dect2020
{

TypeId
EndDeviceDect2020Mac::GetTypeId()
{
    static TypeId tid = TypeId("ns3::EndDeviceDect2020Mac");
}

Dect2020MacMultiplexingHeaderShortSdu::~Dect2020MacMultiplexingHeaderShortSdu()
{
}

}   // namespace dect2020

}
