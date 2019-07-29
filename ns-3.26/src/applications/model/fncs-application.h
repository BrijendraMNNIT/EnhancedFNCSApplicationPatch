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

#ifndef FNCS_APPLICATION_H
#define FNCS_APPLICATION_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable-stream.h"

#include <string>
#include<vector>
#include<cmath>
namespace ns3 {

class Socket;
class Packet;
class InetSocketAddress;
class Inet6SocketAddress;

/**
 * \ingroup fncsapplication
 * \brief A Fncs Application
 *
 * Every packet sent should be returned by the server and received here.
 */
class FncsApplication : public Application 
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  FncsApplication ();

  virtual ~FncsApplication ();

  /**
   * \brief set the name
   * \param name name
   */
  void SetName (const std::string &name);
  /**
   * \brief set the local address and port
   * \param ip local IPv4 address
   * \param port local port
   */
  void SetLocal (Ipv4Address ip, uint16_t port);
  /**
   * \brief set the local address and port
   * \param ip local IPv6 address
   * \param port local port
   */
  void SetLocal (Ipv6Address ip, uint16_t port);
  /**
   * \brief set the local address and port
   * \param ip local IP address
   * \param port local port
   */
  void SetLocal (Address ip, uint16_t port);

  std::string GetName (void) const;

  InetSocketAddress GetLocalInet (void) const;

  Inet6SocketAddress GetLocalInet6 (void) const;
   
   /* ESPServer Function declaration */
void ESPServerFunction(void);
void charging_to_do_nothing(std::string, Ptr<FncsApplication> );
void discharging_to_do_nothing(std::string, Ptr<FncsApplication>); 
void charging(std::string, Ptr<FncsApplication>);
void discharging(std::string, Ptr<FncsApplication>);
  /**
   * \brief Handle a packet creation based on FNCS data.
   *
   * This function is called by the FncsSimulator.
   *
   * \param to the destination application
   * \param topic the topic string
   * \param value the associated value
   */
  void Send (Ptr<FncsApplication> to, std::string topic, std::string value);


        /* This function is called by the FNCSSimulator to update the currentSimulationTime */
        void SetCurrentSimulationTimeInApplicationObject(uint64_t c_s_time);
        uint64_t GetCurrentSimulationTimeInApplicationObject ();
protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);
  
  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  std::string m_name; //!< name of this application
  uint32_t m_sent; //!< Counter for sent packets
  Ptr<Socket> m_socket; //!< Socket
  Address m_localAddress; //!< Local address
  uint16_t m_localPort; //!< Local port
  Ptr<UniformRandomVariable> m_rand_delay_ns; //!<random value used to add jitter to packet transmission
  double m_jitterMinNs; //!<minimum jitter delay time for packets sent via FNCS
  double m_jitterMaxNs; //!<maximum jitter delay time for packets sent via FNCS
  std::string f_name; //!< name of the output file

  /// Callbacks for tracing the packet Tx events
  TracedCallback<Ptr<const Packet> > m_txTrace;
  
        /* variable to hold simulation time in the each application object */
        uint64_t currentSimulationTime; // not used, so it can be removed
        std::vector<double> homeBatteryEnergyVSESPortion = std::vector<double>(6); /* battery capacity of VSES at each home */
        double VSESCapacity; /* Virtual Shared Energy Storage */
        std::vector<double> stateOfChargeVSESPortion = std::vector<double>(6); /*state of charge of the battery (VSES) at each home*/
        
        std::vector<std::string> prosumeridentity = std::vector<std::string>(6);
	std::vector<int> prosumerlocation_x = std::vector<int>(6);
	std::vector<int> prosumerlocation_y = std::vector<int>(6);
        std::vector<std::vector<double> > distances = std::vector<std::vector<double> >(6);
        std::vector<double> distanceofprosumersfromesp = std::vector<double>(6);
};

} // namespace ns3

#endif /* FNCS_APPLICATION_H */
