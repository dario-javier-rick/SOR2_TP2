// Sistemas Operativos y Redes 2
// TCP/IP: Dumbell topology
// Alumnos: Nicolas Cabral, Dar�o Rick
// Profesor: Alexis Tcach
//
//
//	e1 -                           - r1
//		|                         |
//	e2 ------- n0 ------ n1 -------- r2
//		|                         |
//	e3 -                           - r3
//
//
// - 1. Dise�ar un escenario con 3 emisores on/off application, 3 receptores
//  y dos nodo intermedios. O sea se conectar�n los 3 emisores
//  a un nodo, luego �ste a otro y finalmente �ste a los 3 destinos
//  finales. Esto es normalmente llamado Dumbell topology. En el
//  sistema deber� tener uno de los emisores UDP y los otros 2 TCP.
//  Hacer pruebas s�lo con los 2 emisores TCP. Hacer que sature el
//  canal. Medir esa velocidad de transferencia (la cantidad de paquetes
//  que llegan a destino). Mostrar mediante gr�ficos, tama�o
//  de colas de recepci�n, ventana de TCP y cualquier mecanismo
//  que muestre lo que sucede. Explicar en el gr�fico las distintas
//  etapas del protocolo TCP. Usar siempre conexiones cableadas.
// - 2. Calcular el ancho de banda del canal. Explicar qu� sucede. �Ve
//  alguna anomal�a? Explicarla.
// - 3. Posteriormente, con esa misma configuraci�n, pero ahora tambi�n
//  emitiendo el nodo UDP. Explicar qu� sucede con el ancho
//  de banda utilizado por cada uno.
// - 4. Mostrar en los paquetes TCP d�nde se ven las distintas acciones
//  del protocolo.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("dumbell-topology");

int
main (int argc, char *argv[])
{

  bool tracing = false;
  uint32_t maxBytes = 0;

//
// Flags para logs y tama�o de paquete
//
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag de logeo", tracing);
  cmd.AddValue ("maxBytes", "Cantidad de bytes de la prueba", maxBytes);
  cmd.Parse (argc, argv);

//
// Se crean los nodos seg�n la documentaci�n detallada al principio
//
  NS_LOG_INFO ("Creando nodos.");
  NodeContainer nodos;
  NodeContainer emisores;
  NodeContainer receptores;

  nodos.Create(2);
  emisores.Create(3);
  receptores.Create(3);

//
// Se crean enlaces entre los nodos
//
  NS_LOG_INFO ("Creando canales.");
  PointToPointHelper p2pIntermedio, p2p_1, p2p_2, p2p_3, p2p_4, p2p_5, p2p_6;
  NetDeviceContainer ndIntermedios = p2pIntermedio.Install(nodos);
  NetDeviceContainer ndEmisores, ndReceptores;

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodos);

//
// Le doy el stack de internet a todos los nodos
//
  InternetStackHelper internet;
  internet.Install(nodos);
  internet.Install(emisores);
  internet.Install(receptores);

//Asigno Ips
 NS_LOG_INFO ("Asignando IPs.");
 Ipv4AddressHelper routerIp = Ipv4AddressHelper("10.3.0.0","255.255.255.0");
 Ipv4InterfaceContainer routerIFC;
 routerIFC = routerIp.Assign(ndIntermedios);
 /*
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);
*/

//
// Create a BulkSendApplication and install it on node 0
//
  uint16_t port = 9;  // well-known echo port number
/*
  BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (i.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps = source.Install (nodos.Get (0));
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (10.0));
*/

//
// Create a PacketSinkApplication and install it on node 1
//
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (nodos.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (10.0));

//
// Set up tracing if enabled
//
  if (tracing)
    {
      AsciiTraceHelper ascii;
      pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("tcp-bulk-send.tr"));
      pointToPoint.EnablePcapAll ("tcp-bulk-send", false);
    }

//
// Ejecucion de simulacion.
//

  AnimationInterface anim ("test.xml");

  NS_LOG_INFO ("Ejecutando simulacion.");
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Listo.");

  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
 // std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;

}
