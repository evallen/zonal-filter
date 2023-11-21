//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

import inet.linklayer.ethernet.common.BridgingLayer;

import inet.protocolelement.contract.IProtocolLayer;
import inet.protocolelement.processing.IProcessingDelayLayer;
import inet.protocolelement.processing.ProcessingDelayLayer;

//
// An extension of the original BridgingLayer that 
// includes a content-based filter to act as a firewall. 
//
module FirewallBridgingLayer extends BridgingLayer
{
    submodules:
        firewallProcessingDelayLayer: <default("ProcessingDelayLayer")> like IProcessingDelayLayer {
            @display("p=420,1268");
        }
        firewallLayer: <default("FirewallFilterLayer")> like IProtocolLayer {
            @display("p=420,1367");
        }
    connections:
        lowerLayerIn --> { @reconnect; } --> firewallLayer.lowerLayerIn;
        firewallLayer.upperLayerOut --> firewallProcessingDelayLayer.lowerLayerIn;
        firewallProcessingDelayLayer.upperLayerOut --> { @reconnect; } --> vlanPolicy.lowerLayerIn;

        vlanPolicy.lowerLayerOut --> { @reconnect; } --> firewallProcessingDelayLayer.upperLayerIn;
        firewallProcessingDelayLayer.lowerLayerOut --> firewallLayer.upperLayerIn;
        firewallLayer.lowerLayerOut --> { @reconnect; } --> lowerLayerOut;
}
