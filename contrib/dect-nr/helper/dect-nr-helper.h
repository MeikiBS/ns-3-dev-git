// #ifndef DECT_NR_HELPER_H
// #define DECT_NR_HELPER_H

// #include "ns3/dect-nr.h"
// #include <ns3/node-container.h>
// #include <ns3/trace-helper.h>

// #include "../model/dect-phy.h"
// #include "../model/dect-mac.h"

// namespace ns3
// {
// namespace dect2020
// {

// class DectNrHelper
// {
//   public:
//     DectNrHelper();
//     ~DectNrHelper();

//     DectNrHelper(bool useMultiModelSpectrumChannel);

//     Ptr<SpectrumChannel> GetChannel();
//     void SetChannel(Ptr<SpectrumChannel> channel);

//     void AddMobility(Ptr<DectPhy> phy, Ptr<MobilityModel> mobility);

//     NetDeviceContainer Install(NodeContainer c);

//     private:
//     Ptr<SpectrumChannel> m_channel;
// };

// } // namespace dect2020
// } // namespace ns3

// #endif /* DECT_NR_HELPER_H */
