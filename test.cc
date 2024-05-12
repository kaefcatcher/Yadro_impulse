#include <ns3/applications-module.h>
#include <ns3/buildings-module.h>
#include <ns3/config-store-module.h>
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/lte-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-helper.h>

using namespace ns3;

int main(int argc, char *argv[])
{
    // LTE setup
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    NodeContainer enbNodes;
    enbNodes.Create(1);
    NodeContainer ueNodes;
    ueNodes.Create(2);

    // Mobility setup
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ueNodes);

    // LTE device installation
    NetDeviceContainer enbDevs;
    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice(ueNodes);
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    // LTE bearer activation
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);


    // Traffic generation
    uint16_t dlPort = 10000;
    uint16_t ulPort = 20000;
    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        Ptr<Node> ue = ueNodes.Get(u);
        InternetStackHelper stack;
        stack.Install(ue);

        // Downlink traffic
        Ptr<Socket> dlSocket = Socket::CreateSocket(ue, UdpSocketFactory::GetTypeId());
        InetSocketAddress dlRemoteAddr(Ipv4Address("1.0.0.1"), dlPort++);
        dlSocket->Connect(dlRemoteAddr);
        OnOffHelper dlClientHelper("ns3::UdpSocketFactory", dlRemoteAddr);
        dlClientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=10]"));
        dlClientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer dlClientApps = dlClientHelper.Install(ue);
        dlClientApps.Start(Seconds(0.01));

        // Uplink traffic
        Ptr<Socket> ulSocket = Socket::CreateSocket(ue, UdpSocketFactory::GetTypeId());
        InetSocketAddress ulLocalAddr(Ipv4Address::GetAny(), ulPort++);
        ulSocket->Bind(ulLocalAddr);
        InetSocketAddress ulRemoteAddr(Ipv4Address("1.0.0.1"), ulPort++);
        OnOffHelper ulClientHelper("ns3::UdpSocketFactory", ulRemoteAddr);
        ulClientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=10]"));
        ulClientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer ulClientApps = ulClientHelper.Install(ue);
        ulClientApps.Start(Seconds(0.01)); 
    }

    // LTE scheduler setup
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");

    // Enable MAC and RLC traces
    lteHelper->EnableMacTraces();
    lteHelper->EnableRlcTraces();

    
    // Stop and run the simulator
    Simulator::Stop(Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}