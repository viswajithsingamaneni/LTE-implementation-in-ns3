/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-helper.h>
//#include "ns3/gtk-config-store.h"

using namespace ns3;

int main (int argc, char *argv[])
{	
  CommandLine cmd;
  cmd.Parse (argc, argv);
	
  // to save a template default attribute file run it like this:
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim
  //
  // to load a previously created default attribute file
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  // Parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  // Uncomment to enable logging
 //lteHelper->EnableLogComponents ();

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes0;
  NodeContainer ueNodes1;
  enbNodes.Create (2);
  ueNodes0.Create (3);
  ueNodes1.Create (4);

  // Install Mobility Model
  MobilityHelper mobility;

  Ptr<ListPositionAllocator> positionAlloc= CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0,2.0,5.0));
  positionAlloc->Add (Vector (30.0,2.0,10.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (enbNodes);

  BuildingsHelper::Install (enbNodes);
  
  
  Ptr<ListPositionAllocator> positionAlloc1= CreateObject<ListPositionAllocator> ();
  positionAlloc1->Add (Vector (1.0,2.0,2.0));
  positionAlloc1->Add (Vector (2.0,2.0,4.0));
  positionAlloc1->Add (Vector (3.0,2.0,6.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc1);
  mobility.Install (ueNodes0);
  
  BuildingsHelper::Install (ueNodes0);

  Ptr<ListPositionAllocator> positionAlloc2= CreateObject<ListPositionAllocator> ();
  positionAlloc2->Add (Vector (31.0,2.0,1.0));
  positionAlloc2->Add (Vector (32.0,2.0,2.0));
  positionAlloc2->Add (Vector (33.0,2.0,3.0));
  positionAlloc2->Add (Vector (34.0,2.0,4.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc2);
  mobility.Install (ueNodes1);

  BuildingsHelper::Install (ueNodes1);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs0;
  NetDeviceContainer ueDevs1;
  // Default scheduler is PF, uncomment to use RR
  //lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");

  enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  ueDevs0 = lteHelper->InstallUeDevice (ueNodes0);
  ueDevs1 = lteHelper->InstallUeDevice (ueNodes1);

  // Attach a UE to a eNB
  lteHelper->Attach (ueDevs0, enbDevs.Get (0));
  lteHelper->Attach (ueDevs1, enbDevs.Get (1));


  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs0, bearer);
  lteHelper->ActivateDataRadioBearer (ueDevs1, bearer);
  lteHelper->EnableTraces ();

  Simulator::Stop (Seconds (1.05));
  lteHelper->EnablePhyTraces();
  lteHelper->EnableMacTraces();
  Simulator::Run ();

  // GtkConfigStore config;
  // config.ConfigureAttributes ();

  Simulator::Destroy ();
  return 0;
}
