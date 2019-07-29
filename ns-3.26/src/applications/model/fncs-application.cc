/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/names.h"
#include "ns3/string.h"
#include "fncs-application.h"
#include "ns3/random-variable-stream.h"

#ifdef FNCS
#include <fncs.hpp>
#endif

#include <algorithm>
#include <sstream>
#include <string>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FncsApplication");

NS_OBJECT_ENSURE_REGISTERED (FncsApplication);

TypeId
FncsApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FncsApplication")
    .SetParent<Application> ()
    .AddConstructor<FncsApplication> ()
    .AddAttribute ("Name", 
                   "The name of the application",
                   StringValue (),
                   MakeStringAccessor (&FncsApplication::m_name),
                   MakeStringChecker ())
    .AddAttribute ("Sent", 
                   "The counter for outbound packets",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FncsApplication::m_sent),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("LocalAddress", 
                   "The source Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&FncsApplication::m_localAddress),
                   MakeAddressChecker ())
    .AddAttribute ("LocalPort", 
                   "The source port of the outbound packets",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FncsApplication::m_localPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("JitterMinNs",
                   "The source port of the outbound packets",
                   DoubleValue  (1000),
                   MakeDoubleAccessor (&FncsApplication::m_jitterMinNs),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("JitterMaxNs",
                   "The source port of the outbound packets",
                   DoubleValue (100000),
                   MakeDoubleAccessor (&FncsApplication::m_jitterMaxNs),
                   MakeDoubleChecker<double> ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&FncsApplication::m_txTrace),
                     "ns3::Packet::TracedCallback")
    .AddAttribute ("OutFileName", 
                   "The name of the output file",
                   StringValue (),
                   MakeStringAccessor (&FncsApplication::f_name),
                   MakeStringChecker ())
  ;
  return tid;
}

FncsApplication::FncsApplication ()
{
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_socket = 0;
  currentSimulationTime = 0;
  //Setting up jitter random variable stream
  m_rand_delay_ns = CreateObject<UniformRandomVariable> ();
  m_rand_delay_ns->SetAttribute ("Min", DoubleValue  (m_jitterMinNs));
  m_rand_delay_ns->SetAttribute ("Max", DoubleValue  (m_jitterMaxNs));
}

FncsApplication::~FncsApplication()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void
FncsApplication::SetName (const std::string &name)
{
  NS_LOG_FUNCTION (this << name);
  m_name = name;
  Names::Add("fncs_"+name, this);
}

std::string
FncsApplication::GetName (void) const
{
  return m_name;
}

void 
FncsApplication::SetLocal (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_localAddress = ip;
  m_localPort = port;
}

void 
FncsApplication::SetLocal (Ipv4Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_localAddress = Address (ip);
  m_localPort = port;
}

void 
FncsApplication::SetLocal (Ipv6Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_localAddress = Address (ip);
  m_localPort = port;
}

void
FncsApplication::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

/*

int lowestpriceslot(int j)
{
	int priceminindex =0;
	double pricemin = lambda_gb_h[0];
	for (int i=1; i<=j; i++)
	{
		if(pricemin >= lambda_gb_h[i])
		{
			pricemin = lambda_gb_h[i];
			priceminindex = i;
		}
	}
return priceminindex;
}
*/
void
FncsApplication::charging_to_do_nothing(string bat, Ptr<FncsApplication> client)
{
      
        string topic1="house1gld/ESPServer@"+bat+"/chargeonthreshold";
        string topic2="house1gld/ESPServer@"+bat+"/chargeoffthreshold";
        string topic3="house1gld/ESPServer@"+bat+"/dischargeonthreshold";
        string topic4="house1gld/ESPServer@"+bat+"/dischargeoffthreshold";        
 
        Simulator::Schedule(Seconds(0.0),&FncsApplication::Send, this, client, topic1, "100");
        Simulator::Schedule(Seconds(0.000001),&FncsApplication::Send, this, client, topic2, "200");
        
}

void
FncsApplication::discharging_to_do_nothing(string bat, Ptr<FncsApplication> client)
{
      
        string topic1="house1gld/ESPServer@"+bat+"/chargeonthreshold";
        string topic2="house1gld/ESPServer@"+bat+"/chargeoffthreshold";
        string topic3="house1gld/ESPServer@"+bat+"/dischargeonthreshold";
        string topic4="house1gld/ESPServer@"+bat+"/dischargeoffthreshold";        
 
        Simulator::Schedule(Seconds(0.0),&FncsApplication::Send, this, client, topic3, "1000000");
        Simulator::Schedule(Seconds(0.000001),&FncsApplication::Send, this, client, topic4, "900000");
               
}

void
FncsApplication::charging(string bat, Ptr<FncsApplication> client)
{
        string topic1="house1gld/ESPServer@"+bat+"/chargeonthreshold";
        string topic2="house1gld/ESPServer@"+bat+"/chargeoffthreshold";
        string topic3="house1gld/ESPServer@"+bat+"/dischargeonthreshold";
        string topic4="house1gld/ESPServer@"+bat+"/dischargeoffthreshold";        
        
        
        Simulator::Schedule(Seconds(0.0),&FncsApplication::Send, this, client, topic2, "810000");
        Simulator::Schedule(Seconds(0.000001),&FncsApplication::Send, this, client, topic1, "800000");
        
       
}
void
FncsApplication::discharging(string bat, Ptr<FncsApplication> client)
{
        string topic1="house1gld/ESPServer@"+bat+"/chargeonthreshold";
        string topic2="house1gld/ESPServer@"+bat+"/chargeoffthreshold";
        string topic3="house1gld/ESPServer@"+bat+"/dischargeonthreshold";
        string topic4="house1gld/ESPServer@"+bat+"/dischargeoffthreshold";        
      
        Simulator::Schedule(Seconds(0.0),&FncsApplication::Send, this, client, topic4, "250");        
        Simulator::Schedule(Seconds(0.000001),&FncsApplication::Send, this, client, topic3, "300");
        
}




void
FncsApplication::ESPServerFunction(void)
{
NS_LOG_FUNCTION (this);
static int countdays=0;

        Ptr<FncsApplication> market1 = Names::Find<FncsApplication>("fncs_Market_1");
        if(!market1)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  Market_1");
        }

        Ptr<FncsApplication> client1 = Names::Find<FncsApplication>("fncs_t_n_2_right_tn1_tm1_house_in_H_B_ESP1");
        if(!client1)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  fncs_t_n_2_right_tn1_tm1_house_in_H_B_ESP1");
        }
        Ptr<FncsApplication> client2 = Names::Find<FncsApplication>("fncs_t_n_2_right_tn2_tm1_house_in_H_B_ESP2");
        if(!client2)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  fncs_t_n_2_right_tn2_tm1_house_in_H_B_ESP2");
        }
        Ptr<FncsApplication> client3 = Names::Find<FncsApplication>("fncs_t_n_2_right_tn3_tm1_house_in_H_B_ESP3");
        if(!client3)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  fncs_t_n_2_right_tn3_tm1_house_in_H_B_ESP3");
        }
        Ptr<FncsApplication> client4 = Names::Find<FncsApplication>("fncs_t_n_2_right_tn4_tm1_house_in_H_B_ESP4");
        if(!client4)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  fncs_t_n_2_right_tn4_tm1_house_in_H_B_ESP4");
        }
        Ptr<FncsApplication> client5 = Names::Find<FncsApplication>("fncs_t_n_2_right_tn5_tm1_house_in_H_B_ESP5");
        if(!client5)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  fncs_t_n_2_right_tn5_tm1_house_in_H_B_ESP5");
        }
        Ptr<FncsApplication> client6 = Names::Find<FncsApplication>("fncs_t_n_2_right_tn6_tm1_house_in_H_B_ESP6");
        if(!client6)
        {
        NS_FATAL_ERROR("failed FncsApplication lookup  fncs_t_n_2_right_tn6_tm1_house_in_H_B_ESP6");
        }

countdays = countdays +1; // counting simulation days
        
        vector<vector<double> > prosumergeneratesE_pi_h(6); /* energy generation from each prosumer each hour*/
        vector<vector<double> > prosumerconsumesx_pi_h(6); /* energy consumption by each prosumer each hour*/
        
       vector<vector<double> > k_pi_h(6); /* Preference parameter of the prosumers ** values are uniformaly distributed random variable from range [90,150]** */
        vector<double> bc(24); /* variable to holds the amount of energy to be purchased from grid */
        vector<int> cs(24); /* charging slot */

       /* day-ahead real-time price from grid */
        vector<double> lambda_gs_h(24);  /* electrical grid buys energy at this price from ESP */
        vector<double> lambda_gb_h(24);  /* electrical grid sells its energy to the ESP */

         vector<double> allE_h(24);  /* predicted total energy generation by each prosumer in each hour  */
	vector<double> allx_h(24); /* predicted total energy consumption by each prosumer in each hour  */

	vector<double> cumulative_allE_h(24);
	vector<double> cumulative_allx_h(24);

        /* reading generation, consumption, preference parameter of each prosumer of each hour, and day-ahead grid price from input file*/
        string inputFName;
        inputFName = "day"+to_string(countdays)+".csv";

        fstream fin;
        fin.open(inputFName, ios::in);

        if(fin)
        std::cout << "input data file is opened successfully" <<endl;
	else
	std::cout << "input data file is not opened successfully" <<endl;

        int count=0;
        vector<string> row;
        

        for(int i=0; i<24; i++)
        {
                row.clear();
                string line;
                getline(fin, line);
                stringstream lineStream(line);
                string cell;
                while(std::getline(lineStream,cell,','))
                {
                        row.push_back(cell);
                }
                int j=0;
                for(j=0; j<6;j++)
                {
                         
                        prosumergeneratesE_pi_h[j].push_back(std::stod(row[j])); 
			prosumerconsumesx_pi_h[j].push_back(std::stod(row[j+6]));
			k_pi_h[j].push_back(std::stod(row[j+12]));
        
                     //    prosumergeneratesE_pi_h[j][i] = stod(row[j]);        
                     //    prosumerconsumesx_pi_h[j][i] = stod(row[j+6]); 
                      //   k_pi_h[j][i] = stod(row[j+12]);
                }
                lambda_gs_h[i]= stod(row[18]);
                lambda_gb_h[i]= stod(row[19]);
        }           

      // calculate day-ahead charging schedule
       /* calculating day-ahead energy generation by all the prosumers */
	for(int i=0; i < 24;i++)
	{
		double temp=0.0;
		
		for(int j =0; j <6; j++)
		{
			temp = temp + prosumergeneratesE_pi_h[j][i];
		}

		allE_h[i] = temp;
	} 
       /* calculating day-ahead  energy consumption by all the prosumers */
	for(int i=0; i < 24;i++)
	{
		double temp=0.0;
		
		for(int j =0; j <6; j++)
		{
			temp = temp + prosumerconsumesx_pi_h[j][i];
		}

		allx_h[i] = temp;
	}

       /* calculating day-ahead  cumulative energy generation from all the prosumers */
	cumulative_allE_h[0]= allE_h[0];
	for(int i=1; i<24; i++)
	{
		cumulative_allE_h[i] = cumulative_allE_h[i-1] + allE_h[i];

	}
	/* calculating day-ahead cumulative energy consumption from all the prosumers */
	cumulative_allx_h[0]= allx_h[0];
	for(int i=1; i<24; i++)
	{
		cumulative_allx_h[i] = cumulative_allx_h[i-1] + allx_h[i];

	}

        double bcc=0.0; /* temporary variable */
        bc[0]=0.0; 
        for(int k=0; k<24; k++)	cs[k]=-1;
	for(int i=1; i<24; i++)
	{
		if(cumulative_allx_h[i] > cumulative_allE_h[i])
		{
			bc[i-1] = cumulative_allx_h[i] - cumulative_allE_h[i] -bcc;		
                        if(bc[i-1] < 0)
			bc[i-1] =0;
	
			
			
                        int priceminindex =0;
                        {
                                
	                        double pricemin = lambda_gb_h[0];
	                        for (int k=1; k<=(i-1); k++)
	                        {
		                                if(pricemin >= lambda_gb_h[k])
		                                {
                        			pricemin = lambda_gb_h[k];
                        			priceminindex = k;
                                		}
                        	}
                         }
			//cs[i-1] = lowestpriceslot(i-1);
                        if(bc[i-1] > 0)
			cs[i-1] = priceminindex;
                        else 
			cs[i-1] = -1;
                if(cs[i-1]!=-1){
			if (lambda_gb_h[cs[i-1]] > lambda_gb_h[i])
			{
				cs[i-1]=-1;  /* do not charge VSES till i-1th slot */
                                bc[i-1]=0;
			}
                     }
                        bcc = bcc + bc[i-1];
		}
	}

        /* sorting using bubble sort */

	for(int i=0;i<24-1;i++)
	{
		for(int j=0;j<24-i-1;j++)
		{
			if(cs[j] > cs[j+1])
			{
				int temp1 = 0;
				temp1 = cs[j];
				cs[j] = cs[j+1];
				cs[j+1] = temp1;
			
				double temp2 =0.0;
				 temp2 = bc[j];
				 bc[j] = bc[j+1];
				 bc[j+1] = temp2;
			}

		}
	}

        string outputchargingFName;
        outputchargingFName = "chargingscheduleday" + to_string(countdays)+".csv";
        ofstream chargingschedulefile(outputchargingFName, ios::out);
        for(int k=0; k<24; k++)
        chargingschedulefile << cs[k] << "," << bc[k] << "\n";

       chargingschedulefile.close();

      // calculate internal prices and send to the clients
       vector<double> lambda_s_h(24); /* ESP buys energy at this price from prosumers */
       vector<double> lambda_b_h(24); /* ESP sells energy at this price to prosumers */

        vector<double> E_b_h(24); /* Set of seller prosumer total energy PS*/
        vector<double> E_s_h(24); /* Set of buyer prosumer total energy PB*/
        
        vector<double> PB_E_pi_h_1(24); /* sigma (1 + E_pi_h) for PB */
        vector<double> PS_E_pi_h_1(24); /* sigma (1 + E_pi_h) for PS */

        vector<double> PB_k_pi_h(24); 
        vector<double> PS_k_pi_h(24);
 
        for(int i=0; i< 24; i++)
	{
		for(int j=0; j<6; j++)
		{
			if(prosumergeneratesE_pi_h[j][i] > prosumerconsumesx_pi_h[j][i])
			{
				E_b_h[i] = E_b_h[i] + prosumergeneratesE_pi_h[j][i] - prosumerconsumesx_pi_h[j][i];
				PS_E_pi_h_1[i] = PS_E_pi_h_1[i] + (prosumergeneratesE_pi_h[j][i] + 1);
				PS_k_pi_h[i] = PS_k_pi_h[i] + k_pi_h[j][i];
		
			}
			else
			{
				E_s_h[i] = E_s_h[i] + prosumerconsumesx_pi_h[j][i] - prosumergeneratesE_pi_h[j][i];
				PB_E_pi_h_1[i] = PB_E_pi_h_1[i] + (prosumergeneratesE_pi_h[j][i] + 1);
				PB_k_pi_h[i] = PB_k_pi_h[i] + k_pi_h[j][i];
			}
		}
	}
        
        for(int i=0; i<24; i++)
	{
		/* if E_b_h - E_s_h > 0 */
		if((E_b_h[i] - E_s_h[i]) > 0)
		{
			if(PB_E_pi_h_1[i]!=0)
				lambda_b_h[i] = sqrt(lambda_gs_h[i]*PB_k_pi_h[i]/PB_E_pi_h_1[i]); // + some uniform random value
			else
				lambda_b_h[i] = lambda_gb_h[i];
		
			if(PS_E_pi_h_1[i]!=0)
				lambda_s_h[i] = sqrt(lambda_gs_h[i]*PS_k_pi_h[i]/PS_E_pi_h_1[i]); // + some uniform random value
			else
				lambda_s_h[i] = lambda_gs_h[i];
		}
		else
		{
		/* if E_b_h + d_h <  E_s_h  */
			if(PB_E_pi_h_1[i]!=0)
				lambda_b_h[i] = sqrt(lambda_gb_h[i]*PB_k_pi_h[i]/PB_E_pi_h_1[i]); // + some uniform random value
			else
  				lambda_b_h[i] = lambda_gb_h[i];		
			if(PS_E_pi_h_1[i]!=0)
				lambda_s_h[i] = sqrt(lambda_gb_h[i]*PS_k_pi_h[i]/PS_E_pi_h_1[i]); // + some uniform random value
			else
				lambda_s_h[i] = lambda_gs_h[i];
		}
	}

        /*Checking the price constraint in equation 4*/
        for(int i=0; i<24; i++)
        {
                if(lambda_gs_h[i] <= lambda_s_h[i])
                {
                }
                else
                {
                        lambda_s_h[i] = lambda_gs_h[i]; 
                }         

		if(lambda_b_h[i] <= lambda_gb_h[i])
                {
			if(lambda_b_h[i] > lambda_s_h[i])
			{}
			else
			{
			  lambda_b_h[i] = lambda_s_h[i];		
			}
                }
                else
                {
                       lambda_b_h[i] = lambda_gb_h[i]; 
               }  
       
        }
        
        string outputinternalpriceFName;
        outputinternalpriceFName = "internalpriceday" + to_string(countdays)+".csv";
        ofstream inpoutfile(outputinternalpriceFName, ios::out);
        for(int k=0; k<24; k++)
        inpoutfile << lambda_s_h[k] << "," << lambda_b_h[k] << "\n";

        inpoutfile.close();

  // calculate profit  

/* Sending day-ahead charging schedule to the VSES Battery_1, Battery_2, Battery_3, Battery_4, Battery_5, Battery_6 */
     double timespent1=0.0;
        for(int i=0; i<24; i++)
        {      

                        double charging_time = bc[i]/3000; // in seconds
                        double time1 =  (charging_time *14)/100 + timespent1;
                        double time2 =  (charging_time *14)/100 + timespent1;        
                        double time3 =  (charging_time *14)/100 + timespent1;        
                        double time4 =  (charging_time *28)/100 + timespent1;        
                        double time5 =  (charging_time *21)/100 + timespent1;        
                        double time6 =  (charging_time *7)/100 + timespent1;  


                if(cs[i] == i)
                {
                                      

                        stateOfChargeVSESPortion[0] = stateOfChargeVSESPortion[0] + time1*3000;
                        if(stateOfChargeVSESPortion[0] <= homeBatteryEnergyVSESPortion[0])
                        {
                                Simulator::Schedule(Seconds(timespent1), &FncsApplication::charging, this, "t_n_2_right_tn1_tm1_house_in_H_B_ESP1", client1);
                                Simulator::Schedule(Seconds(time1), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn1_tm1_house_in_H_B_ESP1", client1);
                        }
                        stateOfChargeVSESPortion[1] = stateOfChargeVSESPortion[1] + time2*3000;
                        if(stateOfChargeVSESPortion[1] <= homeBatteryEnergyVSESPortion[1])
                        {
                                Simulator::Schedule(Seconds(timespent1), &FncsApplication::charging, this, "t_n_2_right_tn2_tm1_house_in_H_B_ESP2", client2);
                                Simulator::Schedule(Seconds(time2), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn2_tm1_house_in_H_B_ESP2", client2);
                        }

                        stateOfChargeVSESPortion[2] = stateOfChargeVSESPortion[2] + time3*3000;
                        if(stateOfChargeVSESPortion[2] <= homeBatteryEnergyVSESPortion[2])
                        {
                                Simulator::Schedule(Seconds(timespent1), &FncsApplication::charging, this, "t_n_2_right_tn3_tm1_house_in_H_B_ESP3", client3);
                                Simulator::Schedule(Seconds(time3), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn3_tm1_house_in_H_B_ESP3", client3);
                        }
                        stateOfChargeVSESPortion[3] = stateOfChargeVSESPortion[3] + time4*3000;
                        if(stateOfChargeVSESPortion[3] <= homeBatteryEnergyVSESPortion[3])
                        {
                                Simulator::Schedule(Seconds(timespent1), &FncsApplication::charging, this, "t_n_2_right_tn4_tm1_house_in_H_B_ESP4", client4);
                                Simulator::Schedule(Seconds(time4), &FncsApplication::charging_to_do_nothing,this,  "t_n_2_right_tn4_tm1_house_in_H_B_ESP4", client4);
                        }
                        stateOfChargeVSESPortion[4] = stateOfChargeVSESPortion[4] + time5*3000;
                        if(stateOfChargeVSESPortion[4] <= homeBatteryEnergyVSESPortion[4])
                        {
                                Simulator::Schedule(Seconds(timespent1), &FncsApplication::charging, this, "t_n_2_right_tn5_tm1_house_in_H_B_ESP5", client5);
                                Simulator::Schedule(Seconds(time5), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn5_tm1_house_in_H_B_ESP5", client5);
                        }
                        stateOfChargeVSESPortion[5] = stateOfChargeVSESPortion[5] + time6*3000;
                        if(stateOfChargeVSESPortion[5] <= homeBatteryEnergyVSESPortion[5])
                        {
                                Simulator::Schedule(Seconds(timespent1), &FncsApplication::charging, this, "t_n_2_right_tn6_tm1_house_in_H_B_ESP6", client6);
                                Simulator::Schedule(Seconds(time6), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn6_tm1_house_in_H_B_ESP6", client6);
                        }
                
                }
                
        /* battery charging and discharging control for operation*/
                if(E_b_h[i] > 0)        /* charging VSES from prosumers energy*/
                {
                        double tt = E_b_h[i]/3000; //in seconds
                        
                        if(prosumergeneratesE_pi_h[0][i] > prosumerconsumesx_pi_h[0][i])
			{
				double energy = prosumergeneratesE_pi_h[0][i] - prosumerconsumesx_pi_h[0][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[0] = stateOfChargeVSESPortion[0] + t*3000;
                                 if(stateOfChargeVSESPortion[0] <= homeBatteryEnergyVSESPortion[0])
                                 {
                                Simulator::Schedule(Seconds(1 + time1 ), &FncsApplication::charging, this, "t_n_2_right_tn1_tm1_house_in_H_B_ESP1", client1);
                                Simulator::Schedule(Seconds(1 + time1 + t), &FncsApplication::charging_to_do_nothing,this, "t_n_2_right_tn1_tm1_house_in_H_B_ESP1", client1);
                                 }
		
			}
			

                        if(prosumergeneratesE_pi_h[1][i] > prosumerconsumesx_pi_h[1][i])
			{
				double energy = prosumergeneratesE_pi_h[1][i] - prosumerconsumesx_pi_h[1][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[1] = stateOfChargeVSESPortion[1] + t*3000;
                                 if(stateOfChargeVSESPortion[1] <= homeBatteryEnergyVSESPortion[1])
                                 {
                                Simulator::Schedule(Seconds(1 + time2 ), &FncsApplication::charging, this, "t_n_2_right_tn2_tm1_house_in_H_B_ESP2", client2);
                                Simulator::Schedule(Seconds(1 + time2 + t), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn2_tm1_house_in_H_B_ESP2", client2);
                                 }
		
			}
			

                        if(prosumergeneratesE_pi_h[2][i] > prosumerconsumesx_pi_h[2][i])
			{
				double energy = prosumergeneratesE_pi_h[2][i] - prosumerconsumesx_pi_h[2][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[2] = stateOfChargeVSESPortion[2] + t*3000;
                                 if(stateOfChargeVSESPortion[2] <= homeBatteryEnergyVSESPortion[2])
                                 {
                                Simulator::Schedule(Seconds(1 + time3 ), &FncsApplication::charging, this, "t_n_2_right_tn3_tm1_house_in_H_B_ESP3", client3);
                                Simulator::Schedule(Seconds(1 + time3 + t), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn3_tm1_house_in_H_B_ESP3", client3);
                                 }
		
			}
			
			    

                        if(prosumergeneratesE_pi_h[3][i] > prosumerconsumesx_pi_h[3][i])
			{
				double energy = prosumergeneratesE_pi_h[3][i] - prosumerconsumesx_pi_h[3][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[3] = stateOfChargeVSESPortion[3] + t*3000;
                                 if(stateOfChargeVSESPortion[3] <= homeBatteryEnergyVSESPortion[3])
                                 {
                                Simulator::Schedule(Seconds(1 + time4 ), &FncsApplication::charging, this, "t_n_2_right_tn4_tm1_house_in_H_B_ESP4", client4);
                                Simulator::Schedule(Seconds(1 + time4 + t), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn4_tm1_house_in_H_B_ESP4", client4);
                                 }
		
			}
			

                        if(prosumergeneratesE_pi_h[4][i] > prosumerconsumesx_pi_h[4][i])
			{
				double energy = prosumergeneratesE_pi_h[4][i] - prosumerconsumesx_pi_h[4][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[4] = stateOfChargeVSESPortion[4] + t*3000;
                                 if(stateOfChargeVSESPortion[4] <= homeBatteryEnergyVSESPortion[4])
                                 {
                                Simulator::Schedule(Seconds(1 + time5 ), &FncsApplication::charging,this, "t_n_2_right_tn5_tm1_house_in_H_B_ESP5", client5);
                                Simulator::Schedule(Seconds(1 + time5 + t), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn5_tm1_house_in_H_B_ESP5", client5);
                                 }
		
			}
			
			     


                        if(prosumergeneratesE_pi_h[5][i] > prosumerconsumesx_pi_h[5][i])
			{
				double energy = prosumergeneratesE_pi_h[5][i] - prosumerconsumesx_pi_h[5][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[5] = stateOfChargeVSESPortion[5] + t*3000;
                                 if(stateOfChargeVSESPortion[5] <= homeBatteryEnergyVSESPortion[5])
                                 {
                                Simulator::Schedule(Seconds(1 + time6 ), &FncsApplication::charging, this, "t_n_2_right_tn6_tm1_house_in_H_B_ESP6", client6);
                                Simulator::Schedule(Seconds(1 + time6 + t), &FncsApplication::charging_to_do_nothing, this, "t_n_2_right_tn6_tm1_house_in_H_B_ESP6", client6);
                                 }
		
			}
			
			  
               }
                else if(E_s_h[i] > 0)  /* discharging */
                {

                double tt = E_s_h[i]/3000; //in seconds

                if(prosumergeneratesE_pi_h[0][i] < prosumerconsumesx_pi_h[0][i])
                 {
				double energy = prosumerconsumesx_pi_h[0][i] - prosumergeneratesE_pi_h[0][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[0] = stateOfChargeVSESPortion[0] - t*3000;
                                 if(stateOfChargeVSESPortion[0] >= 0)
                                 {
                                Simulator::Schedule(Seconds(1 + time1 ), &FncsApplication::discharging, this, "t_n_2_right_tn1_tm1_house_in_H_B_ESP1", client1);
                                Simulator::Schedule(Seconds(1 + time1 + t), &FncsApplication::discharging_to_do_nothing, this, "t_n_2_right_tn1_tm1_house_in_H_B_ESP1", client1);
                                 }
		}                 

                if(prosumergeneratesE_pi_h[1][i] < prosumerconsumesx_pi_h[1][i])
                {
				double energy = prosumerconsumesx_pi_h[1][i] - prosumergeneratesE_pi_h[1][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[1] = stateOfChargeVSESPortion[1] - t*3000;
                                 if(stateOfChargeVSESPortion[1] >= 0)
                                 {
                                Simulator::Schedule(Seconds(1 + time2 ), &FncsApplication::discharging, this, "t_n_2_right_tn2_tm1_house_in_H_B_ESP2", client2);
                                Simulator::Schedule(Seconds(1 + time2 + t), &FncsApplication::discharging_to_do_nothing, this, "t_n_2_right_tn2_tm1_house_in_H_B_ESP2", client2);
                                 }
		}
                if(prosumergeneratesE_pi_h[2][i] < prosumerconsumesx_pi_h[2][i])
                {
				double energy = prosumerconsumesx_pi_h[2][i] - prosumergeneratesE_pi_h[2][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[2] = stateOfChargeVSESPortion[2] - t*3000;
                                 if(stateOfChargeVSESPortion[2] >= 0)
                                 {
                                Simulator::Schedule(Seconds(1 + time3 ), &FncsApplication::discharging, this, "t_n_2_right_tn3_tm1_house_in_H_B_ESP3", client3);
                                Simulator::Schedule(Seconds(1 + time3 + t), &FncsApplication::discharging_to_do_nothing,this, "t_n_2_right_tn3_tm1_house_in_H_B_ESP3", client3);
                                 }
		}                    
                if(prosumergeneratesE_pi_h[3][i] < prosumerconsumesx_pi_h[3][i])
                {
				double energy = prosumerconsumesx_pi_h[3][i] - prosumergeneratesE_pi_h[3][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[3] = stateOfChargeVSESPortion[3] - t*3000;
                                 if(stateOfChargeVSESPortion[3] >= 0)
                                 {
                                Simulator::Schedule(Seconds(1 + time4 ), &FncsApplication::discharging, this, "t_n_2_right_tn4_tm1_house_in_H_B_ESP4", client4);
                                Simulator::Schedule(Seconds(1 + time4 + t), &FncsApplication::discharging_to_do_nothing, this, "t_n_2_right_tn4_tm1_house_in_H_B_ESP4", client4);
                                 }
		}      

                if(prosumergeneratesE_pi_h[4][i] < prosumerconsumesx_pi_h[4][i])
                {
				double energy = prosumerconsumesx_pi_h[4][i] - prosumergeneratesE_pi_h[4][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[4] = stateOfChargeVSESPortion[4] - t*3000;
                                 if(stateOfChargeVSESPortion[4] >= 0)
                                 {
                                Simulator::Schedule(Seconds(1 + time5 ), &FncsApplication::discharging, this, "t_n_2_right_tn5_tm1_house_in_H_B_ESP5", client5);
                                Simulator::Schedule(Seconds(1 + time5 + t), &FncsApplication::discharging_to_do_nothing, this, "t_n_2_right_tn5_tm1_house_in_H_B_ESP5", client5);
                                 }
		}   
                if(prosumergeneratesE_pi_h[5][i] < prosumerconsumesx_pi_h[5][i])
                {
				double energy = prosumerconsumesx_pi_h[5][i] - prosumergeneratesE_pi_h[5][i];
                                double t = energy/3000; // in seconds
                                stateOfChargeVSESPortion[5] = stateOfChargeVSESPortion[5] - t*3000;
                                 if(stateOfChargeVSESPortion[5] >= 0)
                                 {
                                Simulator::Schedule(Seconds(1 + time6 ), &FncsApplication::discharging,this, "t_n_2_right_tn6_tm1_house_in_H_B_ESP6", client6);
                                Simulator::Schedule(Seconds(1 + time6 + t), &FncsApplication::discharging_to_do_nothing, this, "t_n_2_right_tn6_tm1_house_in_H_B_ESP6", client6);
                                 }
		}                     




                 }

        /*********************/
        timespent1 = timespent1 + 3600;
        }

        /* Sending internal prices to the Market_1*/
      double timeseconds=0.0;
        for(int i=0; i<24; i++)
        {      
        string topic="house1gld/ESPServer@Market_1/currentmarketclearingprice";
        std::ostringstream strs;
        strs << lambda_b_h[i];
        std::string value = strs.str();

        Simulator::Schedule(Seconds(timeseconds),&FncsApplication::Send, this, market1, topic, value);
        timeseconds = timeseconds + 3600;
        }

   



Simulator::Schedule(Seconds(86400.0),&FncsApplication::ESPServerFunction,this);
}


void 
FncsApplication::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_localAddress) == true)
        {
          m_socket->Bind (InetSocketAddress (Ipv4Address::ConvertFrom(m_localAddress), m_localPort));
        }
      else if (Ipv6Address::IsMatchingType(m_localAddress) == true)
        {
          m_socket->Bind (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_localAddress), m_localPort));
        }
      else
        {
          m_socket->Bind();
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&FncsApplication::HandleRead, this));

  if (m_name.empty()) {
    NS_FATAL_ERROR("FncsApplication is missing name");
  }

  if (f_name.empty())
  {
    NS_LOG_INFO("FncsApplication is missing an output file in CSV format.");
  }
        /* Initiating ESPServer and clients*/
        if(m_name=="ESPServer")
        {
                homeBatteryEnergyVSESPortion[0]= 10000*60*60; // in Watt-Second
                homeBatteryEnergyVSESPortion[1]= 10000*60*60;
                homeBatteryEnergyVSESPortion[2]= 10000*60*60;
                homeBatteryEnergyVSESPortion[3]= 20000*60*60;
                homeBatteryEnergyVSESPortion[4]= 15000*60*60;
                homeBatteryEnergyVSESPortion[5]= 5000*60*60;

                VSESCapacity = homeBatteryEnergyVSESPortion[0]+ homeBatteryEnergyVSESPortion[1] + homeBatteryEnergyVSESPortion[2] +    homeBatteryEnergyVSESPortion[3] + homeBatteryEnergyVSESPortion[4] + homeBatteryEnergyVSESPortion[5];

                prosumeridentity[0] = "t_n_2_right_tn1_tm1_house_in_H_B_ESP1";
                prosumeridentity[1] = "t_n_2_right_tn2_tm1_house_in_H_B_ESP2";
                prosumeridentity[2] = "t_n_2_right_tn3_tm1_house_in_H_B_ESP3";
                prosumeridentity[3] = "t_n_2_right_tn4_tm1_house_in_H_B_ESP4";
                prosumeridentity[4] = "t_n_2_right_tn5_tm1_house_in_H_B_ESP5";
                prosumeridentity[5] = "t_n_2_right_tn6_tm1_house_in_H_B_ESP6";

                prosumerlocation_x[0]=170; // in feet
                prosumerlocation_y[0]=-10;

                prosumerlocation_x[1]=330;
                prosumerlocation_y[1]=10;
                
                prosumerlocation_x[2]=490;
                prosumerlocation_y[2]=-10;
        
                prosumerlocation_x[3]=650;
                prosumerlocation_y[3]=10;

                prosumerlocation_x[4]=810;
                prosumerlocation_y[4]=-10;

                prosumerlocation_x[5]=970;
                prosumerlocation_y[5]=10;

                /* calculating distances among prosumers */	
        	for(int i=0; i<6; i++)
        	{
        		for(int j=0; j<6; j++)
        		{
        			double d = sqrt((prosumerlocation_y[i] - prosumerlocation_y[j])*(prosumerlocation_y[i] - prosumerlocation_y[j]) + (prosumerlocation_x[i] - prosumerlocation_x[j])*(prosumerlocation_x[i] - prosumerlocation_x[j]));
                                distances[i].push_back(d);
        		}
        	}

                /* calculating distance of prosumers from ESPServer*/	
	      /* the ESPServer is at point (0,0) */
	        for(int i=0; i<6; i++)
	        {
		distanceofprosumersfromesp[i] = sqrt((prosumerlocation_y[i])*(prosumerlocation_y[i]) + (prosumerlocation_x[i])*(prosumerlocation_x[i]));
		
	        }

          ESPServerFunction();       
              
        }
        else
        {
                
        }
}

void 
FncsApplication::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = 0;
    }
}

static uint8_t
char_to_uint8_t (char c)
{
  return uint8_t(c);
}

// break long topic of structure simname/from@to/topic into its component parts
static std::vector<std::string> splitTopic(std::string topic)
{
  std::vector<std::string> retval;
 
  std::size_t found1 = topic.find_first_of('/');
  std::size_t found2 = topic.find_first_of('/', found1 + 1);
  retval.push_back(topic.substr(0, found1)); // Simulator name
  std::string fromTo = topic.substr(found1 + 1, found2 - found1 - 1); // Source@Destination = SourceLevel_SourceID @ DestinationLevel_DestinationID
  std::size_t found3 = fromTo.find('@');
  // analyzing the source string
  std::string source = fromTo.substr(0, found3); // SourceLevel_SourceID
  std::size_t found4 = source.find_last_of('_');
  std::string sourceLevel = source.substr(0, found4); // Sourcelevel
  std::size_t found5 = sourceLevel.find("Aggregator");
  if (found5 == std::string::npos)
  {
    sourceLevel = "House";
  }
  retval.push_back(sourceLevel);
  retval.push_back(source.substr(found4 + 1, std::string::npos)); // SourceID
  // analyzing the destination string
  std::string destination = fromTo.substr(found3 + 1, std::string::npos); // DestinationLevel_DestinationID
  std::size_t found6 = destination.find_last_of('_');
  std::string destLevel = destination.substr(0, found6); // Destinationlevel
  std::size_t found7 = destLevel.find("Aggregator");
  if (found7 == std::string::npos)
  {
    destLevel = "House";
  }
  retval.push_back(destLevel);
  retval.push_back(destination.substr(found6 + 1, std::string::npos));
  retval.push_back(topic.substr(found2 + 1, std::string::npos)); // small topic name
  
  return retval;
}

void 
FncsApplication::Send (Ptr<FncsApplication> to, std::string topic, std::string value)
{
  NS_LOG_FUNCTION (this << to << topic << value);
 
  Ptr<Packet> p;

  // Convert given value into a Packet.
  size_t total_size = topic.size() + 1 + value.size();
  uint8_t *buffer = new uint8_t[total_size];
  uint8_t *buffer_ptr = buffer;
  std::transform (topic.begin(), topic.end(), buffer_ptr, char_to_uint8_t);
  buffer_ptr += topic.size();
  buffer_ptr[0] = '=';
  buffer_ptr += 1;
  std::transform (value.begin(), value.end(), buffer_ptr, char_to_uint8_t);
  p = Create<Packet> (buffer, total_size);
  NS_LOG_INFO("buffer='" << p << "'");
  delete [] buffer;

  // call to the trace sinks before the packet is actually sent,
  // so that tags added to the packet can be sent as well
  m_txTrace (p);
  
  int delay_ns = (int) (m_rand_delay_ns->GetValue (m_jitterMinNs,m_jitterMaxNs) + 0.5);

  if (Ipv4Address::IsMatchingType (m_localAddress))
    {
      InetSocketAddress address = to->GetLocalInet();
      if (~f_name.empty())
      {
        std::vector<std::string> topicParts = splitTopic(topic);
        std::ofstream outFile(f_name.c_str(), ios::app);
        outFile << Simulator::Now ().GetNanoSeconds ()  << ","
                << p->GetUid () << ","
                << "s,"
                << total_size << ","
                << topicParts[0] << ","
                << topicParts[1] << ","
                << topicParts[2] << ","
                << topicParts[3] << ","
                << topicParts[4] << ","
                << address.GetIpv4() << ","
                << address.GetPort() << ","
                << topicParts[5] << ","
                << value << endl;
        outFile.close();
      }
      NS_LOG_INFO ("At time '"
          << Simulator::Now ().GetNanoSeconds () + delay_ns
          << "'ns '"
          << m_name
          << "' sent "
          << total_size
          << " bytes to '"
          << to->GetName()
          << "' at address "
          << address.GetIpv4()
          << " port "
          << address.GetPort()
		  << " topic '"
          << topic
		  << "' value '"
          << value 
		  << "' uid '"
		  << p->GetUid () <<"'");
      //m_socket->SendTo(p, 0, address);
      //Simulator::Schedule(NanoSeconds (delay_ns), &Socket::SendTo, m_socket, buffer_ptr, p->GetSize(), 0, address); //non-virtual method
      int (Socket::*fp)(Ptr<Packet>, uint32_t, const Address&) = &Socket::SendTo;
      Simulator::Schedule(NanoSeconds (delay_ns), fp, m_socket, p, 0, address); //virtual method
    }
  else if (Ipv6Address::IsMatchingType (m_localAddress))
    {
      Inet6SocketAddress address = to->GetLocalInet6();
      if (~f_name.empty())
      {
        std::vector<std::string> topicParts = splitTopic(topic);
        std::ofstream outFile(f_name.c_str(), ios::app);
        outFile << Simulator::Now ().GetNanoSeconds ()  << ","
                << p->GetUid () << ","
                << "s,"
                << total_size << ","
                << topicParts[0] << ","
                << topicParts[1] << ","
                << topicParts[2] << ","
                << topicParts[3] << ","
                << topicParts[4] << ","
                << address.GetIpv6() << ","
                << address.GetPort() << ","
                << topicParts[5] << ","
                << value << endl;
        outFile.close();
      }
      NS_LOG_INFO ("At time '"
          << Simulator::Now ().GetNanoSeconds () + delay_ns
          << "'ns '"
          << m_name
          << "' sent "
          << total_size
          << " bytes to '"
          << to->GetName()
          << "' at address "
          << address.GetIpv6()
          << " port "
          << address.GetPort()
		  << " topic '"
          << topic
		  << "' value '"
          << value 
		  << "' uid '"
		  << p->GetUid () <<"'");
      //m_socket->SendTo(p, 0, address);
      //Simulator::Schedule(NanoSeconds (delay_ns), &Socket::SendTo, m_socket, buffer_ptr, p->GetSize(), 0, address); //non-virtual method
      int (Socket::*fp)(Ptr<Packet>, uint32_t, const Address&) = &Socket::SendTo;
      Simulator::Schedule(NanoSeconds (delay_ns), fp, m_socket, p, 0, address); //virtual method
    }
  ++m_sent;
}

InetSocketAddress FncsApplication::GetLocalInet (void) const
{
  return InetSocketAddress(Ipv4Address::ConvertFrom(m_localAddress), m_localPort);
}

Inet6SocketAddress FncsApplication::GetLocalInet6 (void) const
{
  return Inet6SocketAddress(Ipv6Address::ConvertFrom(m_localAddress), m_localPort);
}


void
FncsApplication::SetCurrentSimulationTimeInApplicationObject (uint64_t c_s_time)
{
        currentSimulationTime = c_s_time;
}

uint64_t
FncsApplication::GetCurrentSimulationTimeInApplicationObject ()
{
   return    currentSimulationTime;
}


void
FncsApplication::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      uint32_t size = packet->GetSize();
	  std::ostringstream odata;
      packet->CopyData (&odata, size);
      std::string sdata = odata.str();
      size_t split = sdata.find('=', 0);
      if (std::string::npos == split) {
          NS_FATAL_ERROR("HandleRead could not locate '=' to split topic=value");
      }
      std::string topic = sdata.substr(0, split);
      std::string value = sdata.substr(split+1);

       /* code changed here */
    /*  float val = std::stof(value);
       val = val + 1000;
        std::cout << val;
        std::ostringstream vs;
        vs << val;
        value = vs.str();

        uint64_t currenttime;
        currenttime = Simulator::Now().GetNanoSeconds();
        std::cout << currenttime;

        uint64_t fncsapplicationtime;
        fncsapplicationtime = GetCurrentSimulationTimeInApplicationObject ();
        std::cout << "application simulation time " << fncsapplicationtime << endl; */
 /***************************/

      if (InetSocketAddress::IsMatchingType (from))
        {
          if (~f_name.empty())
          {
            std::vector<std::string> topicParts = splitTopic(topic);
            std::ofstream outFile(f_name.c_str(), ios::app);
            outFile << Simulator::Now ().GetNanoSeconds ()  << ","
                    << packet->GetUid () << ","
                    << "r,"
                    << size << ","
                    << topicParts[0] << ","
                    << topicParts[1] << ","
                    << topicParts[2] << ","
                    << topicParts[3] << ","
                    << topicParts[4] << ","
                    << InetSocketAddress::ConvertFrom (from).GetIpv4 () << ","
                    << InetSocketAddress::ConvertFrom (from).GetPort () << ","
                    << topicParts[5] << ","
                    << value << endl;
            outFile.close();
          }
          NS_LOG_INFO ("At time '"
		          << Simulator::Now ().GetNanoSeconds ()
				  << "'ns '"
                                << Simulator::Now ()
				  << "'m_currentTs '"
				  << m_name
                  << "' received "
                  << size
                  << " bytes at address "
                  << InetSocketAddress::ConvertFrom (from).GetIpv4 ()
                  << " port "
                  << InetSocketAddress::ConvertFrom (from).GetPort ()
				  << " topic '"
				  << topic
				  << "' value '"
				  << value
                                  << "' uid '"
		          << packet->GetUid () <<"'");
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          if (~f_name.empty())
          {
            std::vector<std::string> topicParts = splitTopic(topic);
            std::ofstream outFile(f_name.c_str(), ios::app);
            outFile << Simulator::Now ().GetNanoSeconds ()  << ","
                    << packet->GetUid () << ","
                    << "r,"
                    << size << ","
                    << topicParts[0] << ","
                    << topicParts[1] << ","
                    << topicParts[2] << ","
                    << topicParts[3] << ","
                    << topicParts[4] << ","
                    << Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << ","
                    << Inet6SocketAddress::ConvertFrom (from).GetPort () << ","
                    << topicParts[5] << ","
                    << value << endl;
            outFile.close();
          }
          NS_LOG_INFO ("At time '"
		          << Simulator::Now ().GetNanoSeconds ()
				  << "'ns '"
				  << m_name
                  << "' received "
                  << size
                  << " bytes at address "
                  << Inet6SocketAddress::ConvertFrom (from).GetIpv6 ()
                  << " port "
                  << Inet6SocketAddress::ConvertFrom (from).GetPort ()
				  << " topic '"
				  << topic
				  << "' value '"
				  << value
				  << "' uid '"
		          << packet->GetUid () <<"'");
        }
      fncs::publish(topic, value);
        /* code chaged here */
   /*     for(int i=2; i< 5; i++)
        {
                //sleep(11);
               // while(fncsapplicationtime+100 > GetCurrentSimulationTimeInApplicationObject ());
              //  fncsapplicationtime = GetCurrentSimulationTimeInApplicationObject ();
                val = val + 100;
                std::cout << val << "currentSimulationTime: " <<currentSimulationTime<< "GetCurrentSimulationTimeInApplicationObject () " <<GetCurrentSimulationTimeInApplicationObject ();
                std::ostringstream vs;
                vs << val;
                value = vs.str();
                fncs::publish(topic, value);
        } */
        /* */
    }
}

} // Namespace ns3
