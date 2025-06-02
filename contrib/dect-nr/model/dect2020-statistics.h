#ifndef DECT2020_STATISTICS_H
#define DECT2020_STATISTICS_H

#include "ns3/log.h"

#include <cstdint>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

/**
 * @file dect2020-statistics.h
 * @brief Provides a static class to collect and access DECT-2020 NR simulation statistics.
 *
 * This header defines the `Dect2020Statistics` class, which maintains counters for
 * protocol events such as beacon transmissions/receptions, association messages,
 * dropped packets due to RSSI or collisions, and overall transmission counts.
 *
 * It also provides utilities to register and identify packets by UID and type,
 * as well as to print log messages during simulation runs.
 */

namespace ns3
{
/**
 * @class Dect2020Statistics
 * @brief Static class for tracking key simulation statistics in the DECT-2020 NR model.
 *
 * This class centralizes the collection of MAC/PHY-level statistics such as the number
 * of transmitted and received beacons, association messages, and dropped packets.
 *
 * All counters are static, meaning they are global and accessible without creating an instance.
 * This allows simulation-wide data collection across multiple devices.
 *
 * Additionally, the class provides a mechanism to map packet UIDs to human-readable
 * message types for logging or debugging purposes.
 */
class Dect2020Statistics
{
  public:
    static void IncrementClusterBeaconReception();
    static uint32_t GetClusterBeaconReceptionCount();

    static void IncrementClusterBeaconTransmission();
    static uint32_t GetClusterBeaconTransmissionCount();

    static void IncrementPacketsDroppedLowRssi();
    static uint32_t GetPacketsDroppedLowRssiCount();

    static void IncrementPacketsDroppedCollision();
    static uint32_t GetPacketsDroppedCollisionCount();

    static void IncrementSumOfAllPacketsSent();
    static uint32_t GetSumOfAllPacketsSent();

    static void IncrementNetworkBeaconReceptionCount();
    static uint32_t GetNetworkBeaconReceptionCount();

    static void IncrementNetworkBeaconTransmissionCount();
    static uint32_t GetNetworkBeaconTransmissionCount();

    static void IncrementAssociationRequestTransmissionCount();
    static uint32_t GetAssociationRequestTransmissionCount();

    static void IncrementAssociationRequestReceptionCount();
    static uint32_t GetAssociationRequestReceptionCount();

    static void IncrementAssociationResponseTransmissionCount();
    static uint32_t GetAssociationResponseTransmissionCount();

    static void IncrementAssociationResponseReceptionCount();
    static uint32_t GetAssociationResponseReceptionCount();

    static void RegisterPacket(uint32_t packetUid, const std::string& messageType);

    static void LogToConsole(const std::string& message);

    static std::string GetPacketType(uint32_t packetUid);

  private:
    static uint32_t m_clusterBeaconReceptionCount;
    static uint32_t m_clusterBeaconTransmissionCount;

    static uint32_t m_packetsDroppedLowRssi;
    static uint32_t m_packetsDroppedCollision;
    static uint32_t m_sumOfAllPacketsSent;

    static uint32_t m_networkBeaconReceptionCount;
    static uint32_t m_networkBeaconTransmissionCount;

    static uint32_t m_associationRequestReceptionCount;
    static uint32_t m_associationRequestTransmissionCount;

    static uint32_t m_associationResponseReceptionCount;
    static uint32_t m_associationResponseTransmissionCount;

    static std::map<uint32_t, std::string> m_packetTypes;
};
} // namespace ns3

#endif // DECT2020_STATISTICS_H
