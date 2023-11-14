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
            @display("p=375,300");
        }
        rearRightZG: <> like IEthernetNetworkNode {
            @display("p=375,400");
        }
        masterClock: <> like IEthernetNetworkNode if typename != "" {
            @display("p=655,464;i=device/card");
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
            @display("p=772,437;i=device/card");
        }
        head: <> like IEthernetNetworkNode {
            @display("p=805,343;i=device/card");
        }
        mdps: <> like IEthernetNetworkNode {
            @display("p=744,275;i=device/card");
        }
        adas: <> like IEthernetNetworkNode {
            @display("p=608,257;i=device/card");
        }
        gps: <> like IEthernetNetworkNode {
            @display("p=538,273;i=device/card");
        }
        v2x: <> like IEthernetNetworkNode {
            @display("p=507,454;i=device/card");
        }
    connections:
        centralZG.ethg++ <--> Eth1G <--> frontLeftZG.ethg++;
        centralZG.ethg++ <--> Eth1G <--> frontRightZG.ethg++;
        frontLeftZG.ethg++ <--> Eth1G <--> frontRightZG.ethg++;
        rearLeftZG.ethg++ <--> Eth1G <--> rearRightZG.ethg++;
        masterClock.ethg++ <--> Eth100M <--> centralZG.ethg++ if exists(masterClock);
        rearLeftZG.ethg[0] <--> Eth1G <--> centralZG.ethg[0];
        rearRightZG.ethg[0] <--> Eth1G <--> centralZG.ethg[0];
        frontRightCam.ethg[0] <--> Eth100M <--> frontRightZG.ethg[0];
        frontLeftCam.ethg[0] <--> Eth100M <--> frontLeftZG.ethg[0];
        rearLeftCam.ethg[0] <--> Eth100M <--> rearLeftZG.ethg[0];
        rearRightCam.ethg[0] <--> Eth100M <--> rearRightZG.ethg[0];
        rearRightUltrasonic.ethg[0] <--> Eth100M <--> rearRightZG.ethg[0];
        rearLeftUltrasonic.ethg[0] <--> Eth100M <--> rearLeftZG.ethg[0];
        frontLeftUltrasonic.ethg[0] <--> Eth100M <--> frontLeftZG.ethg[0];
        frontRightUltrasonic.ethg[0] <--> Eth100M <--> frontRightZG.ethg[0];
        rearRightZG.ethg[0] <--> Eth100M <--> rearRightWheel.ethg[0];
        rearLeftWheel.ethg[0] <--> Eth100M <--> rearLeftZG.ethg[0];
        frontLeftWheel.ethg[0] <--> Eth100M <--> frontLeftZG.ethg[0];
        frontRightZG.ethg[0] <--> Eth100M <--> frontRightWheel.ethg[0];
        v2x.ethg[0] <--> Eth100M <--> centralZG.ethg[0];
        gps.ethg[0] <--> Eth100M <--> centralZG.ethg[0];
        adas.ethg[0] <--> Eth100M <--> centralZG.ethg[0];
        mdps.ethg[0] <--> Eth100M <--> centralZG.ethg[0];
        head.ethg[0] <--> Eth100M <--> centralZG.ethg[0];
        pcm.ethg[0] <--> Eth100M <--> centralZG.ethg[0];
}
