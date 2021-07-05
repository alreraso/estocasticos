#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/csma-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

//Se define el componente central
//Describe por titulo que es una red ad hoc

NS_LOG_COMPONENT_DEFINE ("Red Ad Hoc Grupo 3");


static void
CourseChangeCallback (std::string path, Ptr<const MobilityModel> model)
{
  Vector posicion = model->GetPosition ();
  std::cout << "CourseChange " << path << " x=" << posicion.x << ", y=" << posicion.y << ", z=" << posicion.z << std::endl;
}

int
main (int argc, char *argv[])
{
  // Inicializacionde variables del sistema
  // algunas como stopTime o infraNodes sirven solo como parametros

  uint32_t backboneNodes = 20;
  uint32_t infraNodes = 1;
  uint32_t lanNodes = 1;
  uint32_t stopTime = 35;
  bool useCourseChangeCallback = false;
  
//Se define el tamaño y la velocidad de transmicion de datos
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue ("1472"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("100kb/s"));

  CommandLine cmd;
  cmd.AddValue ("backboneNodes", "number of backbone nodes", backboneNodes);
  cmd.AddValue ("infraNodes", "number of leaf nodes", infraNodes);
  cmd.AddValue ("lanNodes", "number of LAN nodes", lanNodes);
  cmd.AddValue ("stopTime", "simulation stop time (seconds)", stopTime);
  cmd.AddValue ("useCourseChangeCallback", "whether to enable course change tracing", useCourseChangeCallback);

  cmd.Parse (argc, argv);

  if (stopTime < 15)
    {
      std::cout << "Use a simulation stop time >= 15 seconds" << std::endl;
      exit (1);
    }
  ///////////////////////////////////////////////////////////////////////////
  //                                                                       //
  // Se define el medio de transmision por cable backbone                  //
  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////

  //
  // Se crea el contenedor general de los nodos de red
  //
  NodeContainer backbone;
  backbone.Create (backboneNodes);
  //
  // Se defien y crea el dispositivo de red
  //si cuenta con un ns3 version .19
  // debe descomentar la linea 77 y comentar las lineas 75 y 76
  WifiHelper wifi;
  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate54Mbps"));
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetErrorRateModel ("ns3::NistErrorRateModel");
  //YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer backboneDevices = wifi.Install (wifiPhy, mac, backbone);

  NS_LOG_INFO ("Enabling OLSR routing on all backbone nodes");
  OlsrHelper olsr;
  //
  // Se agregan los protocolos Ipv4 a los nodos de red
  //
  InternetStackHelper internet;
  internet.SetRoutingHelper (olsr); 
  internet.Install (backbone);

  //
  // Se asignan ip alas interfaces de red creadas
  //se utilizo el rango 168.176.125.204 por que es la siguiente alas direcciones de la universidad nacional
  //
  Ipv4AddressHelper ipAddrs;
  ipAddrs.SetBase ("168.176.0.0", "255.255.255.0");
  ipAddrs.Assign (backboneDevices);

  //
  // Se asigna un modelo de movilidad a cada nodo de la red 
  //
  MobilityHelper mobility;
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
                             "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.2]"));
  mobility.Install (backbone);
  
  /*
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  allocator->Add (Vector (1000,0,0));
  allocator->Add (Vector (0,0,0));
  mobility.SetPositionAllocator (allocator);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (backbone);
  */

  if (useCourseChangeCallback == true)
    {
      Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeCallback (&CourseChangeCallback));
    }

  AnimationInterface anim ("red-ad-hoc.xml");

  ///////////////////////////////////////////////////////////////////////////
  //                                                                       //
  // Se inicia la ejecucion dela simulacion de red                         //
  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////

  NS_LOG_INFO ("Run G3 Simulation.");
  Simulator::Stop (Seconds (stopTime));
  Simulator::Run ();
  Simulator::Destroy ();
}
