/* This Code is written by Brijendra Pratap Singh (2014RCS01)
*
* PointToPoint netwok topology n0 represents market and n1 represents ESPServer, n2 to n7 represent homes
*
*             10.1.1.1         10.1.1.2       
*              n0___________________n1               n2           n4       n6
                       ptop           |--------------------------------------------------
                                       -------------------------------------------------- 10.1.2.0  (Ethernet)
*                                                          n3       n5         n7
* network address 10.1.1.0, netmask 255.255.255.0
*
*/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/nix-vector-routing-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/bridge-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "ns3/wimax-helper.h"
#include "ns3/lte-helper.h"
#include "ns3/ipv4.h"
#include "ns3/fncs-application.h"
#include "ns3/fncs-application-helper.h"
#include "ns3/fncs-simulator-impl.h"

#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>



using namespace ns3;
using namespace std;


int main (int argc, char *argv[])
{
   
    int totalNumberOfNodes=8;
    bool verbose = true;
    uint32_t nCsma = 6;
    
    cout << "Total number of nodes is " << totalNumberOfNodes << "." << endl;

    FncsSimulatorImpl *hb=new FncsSimulatorImpl();
    Ptr<FncsSimulatorImpl> hb2(hb);
    hb->Unref();
    Simulator::SetImplementation(hb2);

    LogComponentEnable ("FncsApplication", LOG_LEVEL_INFO);

       
    NodeContainer pToPNodes;
    pToPNodes.Create (2);

    NodeContainer csmaNodes;
    csmaNodes.Add (pToPNodes.Get (1));
    csmaNodes.Create (nCsma);


    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    
    NetDeviceContainer pToPDevices;
    pToPDevices = pointToPoint.Install (pToPNodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (pToPNodes.Get (0));
  stack.Install (csmaNodes);


  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (pToPDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);


/*Application program on ESPServer node*/
ApplicationContainer fncsaps;
	
// Install application on the nodes
	FncsApplicationHelper help("", 0);

	string espServer;
	espServer = "ESPServer";
 
	/* application program at espServer node */	
	fncsaps.Add(help.Install(pToPNodes.Get(1), espServer));

/* Application Programs on the nodes */		
	string marketname;
	marketname = "Market_1";

	string house1;
	house1 = "t_n_2_right_tn1_tm1_house_in_H_B_ESP1";
	
	string house2;
	house2 = "t_n_2_right_tn2_tm1_house_in_H_B_ESP2";
	
	string house3;
	house3 = "t_n_2_right_tn3_tm1_house_in_H_B_ESP3";

	string house4;
	house4 = "t_n_2_right_tn4_tm1_house_in_H_B_ESP4";

	string house5;
	house5 = "t_n_2_right_tn5_tm1_house_in_H_B_ESP5";

	string house6;
	house6 = "t_n_2_right_tn6_tm1_house_in_H_B_ESP6";	
	

	
        
	fncsaps.Add(help.Install(pToPNodes.Get(0), marketname));
	fncsaps.Add(help.Install(csmaNodes.Get(1), house1));
	fncsaps.Add(help.Install(csmaNodes.Get(2), house2));
	fncsaps.Add(help.Install(csmaNodes.Get(3), house3));
	fncsaps.Add(help.Install(csmaNodes.Get(4), house4));
	fncsaps.Add(help.Install(csmaNodes.Get(5), house5));
	fncsaps.Add(help.Install(csmaNodes.Get(6), house6));

        fncsaps.Start (Seconds (0.0));
        fncsaps.Stop (Seconds (604798.0)); // 7 days

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//  pointToPoint.EnablePcapAll ("second");
//  csma.EnablePcap ("second", csmaDevices.Get (1), true);

    Simulator::Run ();
    Simulator::Destroy ();
return 0;
}
