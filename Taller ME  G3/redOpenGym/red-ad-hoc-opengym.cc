#include "ns3/end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/gateway-lorawan-mac.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/lora-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/node-container.h"
#include "ns3/position-allocator.h"
#include "ns3/one-shot-sender-helper.h"
#include "ns3/command-line.h"
#include <algorithm>
#include <ctime>



#include "ns3/core-module.h"
#include "ns3/opengym-module.h"

using namespace ns3;
using namespace lorawan; 

NS_LOG_COMPONENT_DEFINE ("OpenGym");

/*
Define observation space
*/
Ptr<OpenGymSpace> MyGetObservationSpace(void)
{
  uint32_t nodeNum = 20;
  float low = 0.0;
  float high = 10.0;
  std::vector<uint32_t> shape = {nodeNum,};
  std::string dtype = TypeNameGet<uint32_t> ();
  Ptr<OpenGymBoxSpace> space = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);
  NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
  return space;
}

/*
Define action space
*/
Ptr<OpenGymSpace> MyGetActionSpace(void)
{
  uint32_t nodeNum = 20;

  Ptr<OpenGymDiscreteSpace> space = CreateObject<OpenGymDiscreteSpace> (nodeNum);
  NS_LOG_UNCOND ("MyGetActionSpace: " << space);
  return space;
}

/*
Define game over condition
*/
bool MyGetGameOver(void)
{

  bool isGameOver = false;
  bool test = false;
  static float stepCounter = 0.0;
  stepCounter += 1;
  if (stepCounter == 10 && test) {
      isGameOver = true;
  }
  NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
  return isGameOver;
}

/*
Collect observations
*/
Ptr<OpenGymDataContainer> MyGetObservation(void)
{
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.85);
  loss->SetReference (1, 7.4);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

  /************************
  *  Create the helpers  *
  ************************/

  //para est ejemplo se asignan en el chanel posiciones cosntantes dado un vector de 0 a 500

  NS_LOG_INFO ("Setting up helpers...");

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  allocator->Add (Vector (500,0,0));
  allocator->Add (Vector (0,0,0));
  mobility.SetPositionAllocator (allocator);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  /*MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (20.0),
                                 "MinY", DoubleValue (20.0),
                                 "DeltaX", DoubleValue (20.0),
                                 "DeltaY", DoubleValue (20.0),
                                 "GridWidth", UintegerValue (5),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::RandomDirection2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-500, 500, -500, 500)),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=2]"),
                             "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.2]"));*/

  // Create the LoraPhyHelper  aqui asignamos la capa fisica
  LoraPhyHelper phyHelper = LoraPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the LorawanMacHelper aqui asignamos la capa de mac 
  LorawanMacHelper macHelper = LorawanMacHelper ();

  // Create the LoraHelper
  LoraHelper helper = LoraHelper ();

  /************************
  *  Create End Devices  *
  ************************/

  NS_LOG_INFO ("Creating the end device...");

  // Create a set of nodes
  NodeContainer endDevices;
  endDevices.Create (20);

  // Assign a mobility model to the node
  mobility.Install (endDevices);

  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen = CreateObject<LoraDeviceAddressGenerator> (nwkId, nwkAddr);
  macHelper.SetAddressGenerator (addrGen);

  // Create the LoraNetDevices of the end devices    aqui usamos las definiciones de capa fisica y mas para
  //que se identifiquen en el canal los equipos como dispositivos finales o endpoints
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LorawanMacHelper::ED_A);
  helper.Install (phyHelper, macHelper, endDevices);

  /*********************
  *  Create Gateways  *
  *********************/

  NS_LOG_INFO ("Creating the gateway...");
  NodeContainer gateways;
  gateways.Create (1);

  mobility.Install (gateways);

  // Create a netdevice for each gateway
  phyHelper.SetDeviceType (LoraPhyHelper::GW);
  macHelper.SetDeviceType (LorawanMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);

  /*********************************************
  *  Install applications on the end devices  *
  *********************************************/

  OneShotSenderHelper oneShotSenderHelper;
  oneShotSenderHelper.SetSendTime (Seconds (3));

  oneShotSenderHelper.Install (endDevices);

  /******************
   * Set Data Rates *
   ******************/
  std::vector<int> sfQuantity (5);
  sfQuantity = macHelper.SetSpreadingFactorsUp (endDevices, gateways, channel);

  NS_LOG_INFO ("Run Simulation.");
  uint32_t nodeNum = 20;
  uint32_t low = 0.0;
  uint32_t high = 10.0;
  Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();



  std::vector<uint32_t> shape = {nodeNum,};
  Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

  // generate random data
  for (uint32_t i = 0; i<nodeNum; i++){
    uint32_t value = rngInt->GetInteger(low, high);
    box->AddValue(value);
  }

  NS_LOG_UNCOND ("MyGetObservation: " << box);
  return box;
}

/*
Define reward function
*/
float MyGetReward(void)
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}

/*
Definimos informacion extra
*/
std::string MyGetExtraInfo(void)
{
  std::string myInfo = "<== Reward - Done? testInfo";
  myInfo += "|if false ==> next";
  NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
  return myInfo;
}


/*
Execute received actions
*/
bool MyExecuteActions(Ptr<OpenGymDataContainer> action)
{
  Ptr<OpenGymDiscreteContainer> discrete = DynamicCast<OpenGymDiscreteContainer>(action);
  NS_LOG_UNCOND ("MyExecuteActions: " << action);
  return true;
}

void ScheduleNextStateRead(double envStepTime, Ptr<OpenGymInterface> openGym)
{
  Simulator::Schedule (Seconds(envStepTime), &ScheduleNextStateRead, envStepTime, openGym);
  openGym->NotifyCurrentState();
}

int
main (int argc, char *argv[])
{
  // Parameters of the scenario
  uint32_t simSeed = 1;
  double simulationTime = 1; //seconds
  double envStepTime = 0.1; //seconds, ns3gym env step time interval
  uint32_t openGymPort = 9001;
  uint32_t testArg = 0;

  CommandLine cmd;
  // required parameters for OpenGym interface
  cmd.AddValue ("openGymPort", "Port number for OpenGym env. Default: 9001", openGymPort);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", simSeed);
  // optional parameters
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", simulationTime);
  cmd.AddValue ("testArg", "Extra simulation argument. Default: 0", testArg);
  cmd.Parse (argc, argv);

  NS_LOG_UNCOND("Ns3Env parameters:");
  NS_LOG_UNCOND("--simulationTime: " << simulationTime);
  NS_LOG_UNCOND("--openGymPort: " << openGymPort);
  NS_LOG_UNCOND("--envStepTime: " << envStepTime);
  NS_LOG_UNCOND("--seed: " << simSeed);
  NS_LOG_UNCOND("--testArg: " << testArg);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (simSeed);

  // OpenGym Env
  Ptr<OpenGymInterface> openGym = CreateObject<OpenGymInterface> (openGymPort);
  openGym->SetGetActionSpaceCb( MakeCallback (&MyGetActionSpace) );
  openGym->SetGetObservationSpaceCb( MakeCallback (&MyGetObservationSpace) );
  openGym->SetGetGameOverCb( MakeCallback (&MyGetGameOver) );
  openGym->SetGetObservationCb( MakeCallback (&MyGetObservation) );
  openGym->SetGetRewardCb( MakeCallback (&MyGetReward) );
  openGym->SetGetExtraInfoCb( MakeCallback (&MyGetExtraInfo) );
  openGym->SetExecuteActionsCb( MakeCallback (&MyExecuteActions) );
  Simulator::Schedule (Seconds(0.0), &ScheduleNextStateRead, envStepTime, openGym);

  NS_LOG_UNCOND ("Simulation start");
  Simulator::Stop (Seconds (simulationTime));
  Simulator::Run ();
  NS_LOG_UNCOND ("Simulation stop");

  openGym->NotifySimulationEnd();
  Simulator::Destroy ();

}
