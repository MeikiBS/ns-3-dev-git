#ifndef DECT_NET_DEVICE_H
#define DECT_NET_DEVICE_H

#include "ns3/dect-mac.h"
#include "ns3/dect-phy.h"
#include "ns3/log.h"
#include "ns3/net-device.h"
#include "ns3/pointer.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{

class DectNetDevice : public NetDevice
{
  public:
    static TypeId GetTypeId();
    DectNetDevice();
    ~DectNetDevice() override;

    void SetMac(Ptr<DectMac> mac);
    void SetPhy(Ptr<DectPhy> phy);

    // From class NetDevice. Some of these have little meaning for a LoRaWAN
    // network device (since, for instance, IP is not used in the standard)
    void SetReceiveCallback(NetDevice::ReceiveCallback cb) override;
    Ptr<Channel> GetChannel() const override;
    void SetNode(Ptr<Node> node) override;
    Ptr<Node> GetNode() const override;

    Ptr<Dect2020Channel> DoGetChannel() const;
    bool DectNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;

    void SetIfIndex(const uint32_t index) override;
    uint32_t GetIfIndex() const override;
    void SetAddress(Address address) override;
    Address GetAddress() const override;
    bool SetMtu(const uint16_t mtu) override;
    uint16_t GetMtu() const override;
    bool IsLinkUp() const override;
    void AddLinkChangeCallback(Callback<void> callback) override;
    bool IsBroadcast() const override;
    Address GetBroadcast() const override;
    bool IsMulticast() const override;
    Address GetMulticast(Ipv4Address multicastGroup) const override;
    Address GetMulticast(Ipv6Address addr) const override;
    bool IsBridge() const override;
    bool IsPointToPoint() const override;
    bool SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber) override;
    bool NeedsArp() const override;
    void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override;
    bool SupportsSendFrom() const override;

  private:
    // Member variables
    Ptr<Node> m_node;      //!< The Node this NetDevice is connected to.
    Ptr<DectPhy> m_phy;    //!< The LoraPhy this NetDevice is connected to.
    Ptr<DectMac> m_mac;    //!< The LorawanMac this NetDevice is connected to.
    bool m_configComplete; //!< Whether the configuration was already completed.

}; // class DectNetDevice

} // namespace dect2020
} // namespace ns3

#endif /* DECT_NET_DEVICE_H */

