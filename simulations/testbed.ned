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
import inet.node.tsn.TsnDevice;
import ned.IdealChannel;


network Testbed extends TsnNetworkBase
{
    parameters:
        @display("bgi=background/car;bgb=1920,1080");
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
            @display("p=384,273");
        }
        rearRightZG: <> like IEthernetNetworkNode {
            @display("p=384,427");
        }
        masterClock: <> like IEthernetNetworkNode if typename != "" {
            @display("p=608,464;i=device/card");
        }
        rearRightCam: <> like IEthernetNetworkNode {
            @display("p=317,506;i=device/card");
        }
        rearLeftCam: <> like IEthernetNetworkNode {
            @display("p=317,206;i=device/card");
        }
        frontLeftCam: <> like IEthernetNetworkNode {
            @display("p=943,206;i=device/card");
        }
        frontRightCam: <> like IEthernetNetworkNode {
            @display("p=943,512;i=device/card");
        }
        frontRightUltrasonic: <> like IEthernetNetworkNode {
            @display("p=1008,429;i=device/card");
        }
        frontLeftUltrasonic: <> like IEthernetNetworkNode {
            @display("p=1008,273;i=device/card");
        }
        rearRightUltrasonic: <> like IEthernetNetworkNode {
            @display("p=264,437;i=device/card");
        }
        rearLeftUltrasonic: <> like IEthernetNetworkNode {
            @display("p=264,273;i=device/card");
        }
        frontRightWheel: <> like IEthernetNetworkNode {
            @display("p=825,512;i=device/card");
        }
        frontLeftWheel: <> like IEthernetNetworkNode {
            @display("p=825,206;i=device/card");
        }
        rearRightWheel: <> like IEthernetNetworkNode {
            @display("p=433,512;i=device/card");
        }
        rearLeftWheel: <> like IEthernetNetworkNode {
            @display("p=433,206;i=device/card");
        }
        pcm: <> like IEthernetNetworkNode {
            @display("p=744,464;i=device/card");
        }
        head: <> like IEthernetNetworkNode {
            @display("p=805,343;i=device/card");
        }
        mdps: <> like IEthernetNetworkNode {
            @display("p=705,241;i=device/card");
        }
        adas: <> like IEthernetNetworkNode {
            @display("p=599,229;i=device/card");
        }
        gps: <> like IEthernetNetworkNode {
            @display("p=526,241;i=device/card");
        }
        v2x: <> like IEthernetNetworkNode {
            @display("p=507,454;i=device/card");
        }
        rightSpeakers: <> like IEthernetNetworkNode {
            @display("p=324,378;i=device/card");
        }
        leftSpeakers: <> like IEthernetNetworkNode {
            @display("p=324,313;i=device/card");
        }
    connections:
        // Switches
        centralZG.ethg++ <--> Eth1G <--> frontLeftZG.ethg++;
        centralZG.ethg++ <--> Eth1G <--> frontRightZG.ethg++;
        centralZG.ethg++ <--> Eth1G <--> rearLeftZG.ethg++;
        centralZG.ethg++ <--> Eth1G <--> rearRightZG.ethg++;
        frontLeftZG.ethg++ <--> Eth1G <--> frontRightZG.ethg++;
        rearLeftZG.ethg++ <--> Eth1G <--> rearRightZG.ethg++;
        frontLeftZG.ethg++ <--> Eth1G <--> rearLeftZG.ethg++;
        frontRightZG.ethg++ <--> Eth1G <--> rearRightZG.ethg++;

        // ECUs
        masterClock.ethg++ <--> Eth100M <--> centralZG.ethg++ if exists(masterClock);

        frontLeftUltrasonic.ethg++ <--> Eth100M <--> frontLeftZG.ethg++;
        frontRightUltrasonic.ethg++ <--> Eth100M <--> frontRightZG.ethg++;
        rearLeftUltrasonic.ethg++ <--> Eth100M <--> rearLeftZG.ethg++;
        rearRightUltrasonic.ethg++ <--> Eth100M <--> rearRightZG.ethg++;

        frontLeftWheel.ethg++ <--> Eth100M <--> frontLeftZG.ethg++;
        frontRightWheel.ethg++ <--> Eth100M <--> frontRightZG.ethg++;
        rearLeftWheel.ethg++ <--> Eth100M <--> rearLeftZG.ethg++;
        rearRightWheel.ethg++ <--> Eth100M <--> rearRightZG.ethg++;

        frontLeftCam.ethg++ <--> Eth100M <--> frontLeftZG.ethg++;
        frontRightCam.ethg++ <--> Eth100M <--> frontRightZG.ethg++;
        rearLeftCam.ethg++ <--> Eth100M <--> rearLeftZG.ethg++;
        rearRightCam.ethg++ <--> Eth100M <--> rearRightZG.ethg++;

        v2x.ethg++ <--> Eth100M <--> centralZG.ethg++;
        gps.ethg++ <--> Eth100M <--> centralZG.ethg++;
        adas.ethg++ <--> Eth100M <--> centralZG.ethg++;
        mdps.ethg++ <--> Eth100M <--> centralZG.ethg++;
        head.ethg++ <--> Eth100M <--> centralZG.ethg++;
        pcm.ethg++ <--> Eth100M <--> centralZG.ethg++;
        
        rightSpeakers.ethg++ <--> Eth100M <--> rearRightZG.ethg++;
        leftSpeakers.ethg++ <--> Eth100M <--> rearLeftZG.ethg++;
}
