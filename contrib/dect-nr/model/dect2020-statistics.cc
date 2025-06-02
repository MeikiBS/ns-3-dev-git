#include "dect2020-statistics.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("Dect2020Statistics");
uint32_t Dect2020Statistics::m_clusterBeaconReceptionCount = 0;

void
Dect2020Statistics::IncrementClusterBeaconReception()
{
    ++m_clusterBeaconReceptionCount;
}

uint32_t
Dect2020Statistics::GetClusterBeaconReceptionCount()
{
    return m_clusterBeaconReceptionCount;
}

uint32_t Dect2020Statistics::m_clusterBeaconTransmissionCount = 0;

void
Dect2020Statistics::IncrementClusterBeaconTransmission()
{
    ++m_clusterBeaconTransmissionCount;
}

uint32_t
Dect2020Statistics::GetClusterBeaconTransmissionCount()
{
    return m_clusterBeaconTransmissionCount;
}

uint32_t Dect2020Statistics::m_packetsDroppedLowRssi = 0;

void
Dect2020Statistics::IncrementPacketsDroppedLowRssi()
{
    ++m_packetsDroppedLowRssi;
}

uint32_t
Dect2020Statistics::GetPacketsDroppedLowRssiCount()
{
    return m_packetsDroppedLowRssi;
}

uint32_t Dect2020Statistics::m_packetsDroppedCollision = 0;

void
Dect2020Statistics::IncrementPacketsDroppedCollision()
{
    ++m_packetsDroppedCollision;
}

uint32_t
Dect2020Statistics::GetPacketsDroppedCollisionCount()
{
    return m_packetsDroppedCollision;
}

uint32_t Dect2020Statistics::m_sumOfAllPacketsSent = 0;

void
Dect2020Statistics::IncrementSumOfAllPacketsSent()
{
    ++m_sumOfAllPacketsSent;
}

uint32_t
Dect2020Statistics::GetSumOfAllPacketsSent()
{
    return m_sumOfAllPacketsSent;
}

uint32_t Dect2020Statistics::m_networkBeaconReceptionCount = 0;

void
Dect2020Statistics::IncrementNetworkBeaconReceptionCount()
{
    ++m_networkBeaconReceptionCount;
}

uint32_t
Dect2020Statistics::GetNetworkBeaconReceptionCount()
{
    return m_networkBeaconReceptionCount;
}

uint32_t Dect2020Statistics::m_networkBeaconTransmissionCount = 0;

void
Dect2020Statistics::IncrementNetworkBeaconTransmissionCount()
{
    ++m_networkBeaconTransmissionCount;
}

uint32_t
Dect2020Statistics::GetNetworkBeaconTransmissionCount()
{
    return m_networkBeaconTransmissionCount;
}

uint32_t Dect2020Statistics::m_associationRequestTransmissionCount = 0;

void
Dect2020Statistics::IncrementAssociationRequestTransmissionCount()
{
    ++m_associationRequestTransmissionCount;
}

uint32_t
Dect2020Statistics::GetAssociationRequestTransmissionCount()
{
    return m_associationRequestTransmissionCount;
}

uint32_t Dect2020Statistics::m_associationRequestReceptionCount = 0;

void
Dect2020Statistics::IncrementAssociationRequestReceptionCount()
{
    ++m_associationRequestReceptionCount;
}

uint32_t
Dect2020Statistics::GetAssociationRequestReceptionCount()
{
    return m_associationRequestReceptionCount;
}

uint32_t Dect2020Statistics::m_associationResponseTransmissionCount = 0;

void
Dect2020Statistics::IncrementAssociationResponseTransmissionCount()
{
    ++m_associationResponseTransmissionCount;
}

uint32_t
Dect2020Statistics::GetAssociationResponseTransmissionCount()
{
    return m_associationResponseTransmissionCount;
}

uint32_t Dect2020Statistics::m_associationResponseReceptionCount = 0;

void
Dect2020Statistics::IncrementAssociationResponseReceptionCount()
{
    ++m_associationResponseReceptionCount;
}

uint32_t
Dect2020Statistics::GetAssociationResponseReceptionCount()
{
    return m_associationResponseReceptionCount;
}

std::map<uint32_t, std::string> Dect2020Statistics::m_packetTypes;

void
Dect2020Statistics::RegisterPacket(uint32_t packetUid, const std::string& messageType)
{
    m_packetTypes[packetUid] = messageType;
}

void
Dect2020Statistics::LogToConsole(const std::string& message)
{
    NS_LOG_INFO(Simulator::Now().GetMicroSeconds() << " µs: " << message);
}

std::string
Dect2020Statistics::GetPacketType(uint32_t packetUid)
{
    auto it = m_packetTypes.find(packetUid);
    if (it != m_packetTypes.end())
    {
        return it->second;
    }
    return "Unknown";
}
} // namespace ns3
