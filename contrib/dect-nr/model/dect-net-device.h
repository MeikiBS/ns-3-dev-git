#ifndef DECT_NET_DEVICE_H
#define DECT_NET_DEVICE_H

#include "dect-mac.h"
#include "dect-net-device.h"
#include "dect-phy.h"
#include "end-device-dect-nr-mac.h"

#include "ns3/error-model.h"
#include "ns3/log.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/net-device.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/queue.h"
#include "ns3/single-model-spectrum-channel.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup dect-nr Description of the dect-nr
 */

namespace ns3
{
namespace dect2020
{

class DectPhy;
class Dect2020Channel;

class DectNetDevice : public NetDevice
{
  public:
    static TypeId GetTypeId();
    DectNetDevice();
    ~DectNetDevice() override;

    void SetMac(Ptr<DectMac> mac);
    Ptr<DectMac> GetMac() const;
    void SetPhy(Ptr<DectPhy> phy);
    Ptr<Channel> GetChannel() const;
    void SetChannel(Ptr<SpectrumChannel> channel);

    void Test();

    // inherited from NetDevice
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
    Address GetMulticast(Ipv6Address multicastGroup) const override;

    bool IsBridge() const override;

    bool IsPointToPoint() const override;
    bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
    bool SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber);
    Ptr<Node> GetNode() const override;
    void SetNode(Ptr<Node> node) override;
    bool NeedsArp() const override;
    void SetReceiveCallback(ReceiveCallback cb) override;
    void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override;
    bool SupportsSendFrom() const override;

  protected:
    Ptr<DectMac> m_mac;
    Ptr<DectPhy> m_phy;
    Ptr<SpectrumChannel> m_channel;

    Address m_address;

}; // class DectNetDevice

} // namespace dect2020
} // namespace ns3

#endif /* DECT_NET_DEVICE_H */
