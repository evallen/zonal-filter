// 
// Testbed simulation file for Zonal Filtering research
// Nov. 13, 2023
// 
// Author: Evan Allen
//
// Originally adapted from OMNeT++ INET's "InVehicleNetworkShowcase" simulation. 
// in their TSN package.
// 

//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


import inet.networks.base.TsnNetworkBase;
import inet.node.contract.IEthernetNetworkNode;
import inet.node.ethernet.Eth100M;
import ned.IdealChannel;


network Testbed extends TsnNetworkBase
{
    parameters:
        @display("bgi=background/car;bgb=1280,720");
    types:
        channel Eth1G extends inet.node.ethernet.Eth1G
        {
            @display("ls=,3");
        }
    submodules:
        centralZG: <> like IEthernetNetworkNode {
            @display("p=608,355");
        }
        frontLeftZG: <> like IEthernetNetworkNode {
            @display("p=900,275");
        }
        frontRightZG: <> like IEthernetNetworkNode {
            @display("p=900,417");
        }
        rearLeftZG: <> like IEthernetNetworkNode {
            @display("p=375,300");
        }
        rearRightZG: <> like IEthernetNetworkNode {
            @display("p=375,400");
        }
        masterClock: <> like IEthernetNetworkNode if typename != "" {
            @display("p=608,671;i=device/card");
        }
    connections:
        centralZG.ethg++ <--> Eth1G <--> frontLeftZG.ethg++;
        centralZG.ethg++ <--> Eth1G <--> frontRightZG.ethg++;
        frontLeftZG.ethg++ <--> Eth1G <--> frontRightZG.ethg++;
        rearLeftZG.ethg++ <--> Eth1G <--> rearRightZG.ethg++;
        masterClock.ethg++ <--> Eth100M <--> centralZG.ethg++ if exists(masterClock);
        rearLeftZG.ethg[0] <--> IdealChannel <--> centralZG.ethg[0];
}
