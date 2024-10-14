// #include "dect-nr-helper.h"

// namespace ns3
// {
// namespace dect2020
// {
// DectNrHelper::DectNrHelper()
// {
//     m_channel = CreateObject<SingleModelSpectrumChannel>();

//     Ptr<LogDistancePropagationLossModel> lossModel =
//     CreateObject<LogDistancePropagationLossModel>();
//     m_channel->AddPropagationLossModel(lossModel);

//     Ptr<ConstantSpeedPropagationDelayModel> delayModel =
//     CreateObject<ConstantSpeedPropagationDelayModel>();
//     m_channel->SetPropagationDelayModel(delayModel);
// }

// DectNrHelper::DectNrHelper(bool useMultiModelSpectrumChannel)
// {
//     if(useMultiModelSpectrumChannel)
//     {
//         m_channel = CreateObject<MultiModelSpectrumChannel>();
//     }
//     else
//     {
//         m_channel = CreateObject<SingleModelSpectrumChannel>();
//     }
//     m_channel = CreateObject<SingleModelSpectrumChannel>();

//     Ptr<LogDistancePropagationLossModel> lossModel =
//     CreateObject<LogDistancePropagationLossModel>();
//     m_channel->AddPropagationLossModel(lossModel);

//     Ptr<ConstantSpeedPropagationDelayModel> delayModel =
//     CreateObject<ConstantSpeedPropagationDelayModel>();
//     m_channel->SetPropagationDelayModel(delayModel);
// }
// } // namespace dect2020
// } // namespace ns3
