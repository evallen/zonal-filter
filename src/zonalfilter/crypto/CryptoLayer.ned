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

package zonalfilter.crypto;

import inet.protocolelement.contract.IProtocolLayer;
import inet.queueing.contract.IPacketFlow;
import inet.queueing.common.PacketDelayer;
import inet.queueing.base.PacketFlowBase;
import inet.queueing.contract.IPacketDelayer;
import inet.protocolelement.processing.ProcessingDelayLayer;

module CryptoLayer like IProtocolLayer
{
    parameters:
        @display("i=block/layer");
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input lowerLayerIn;
        output lowerLayerOut;
    submodules:
        cryptoAdder: CryptoAdder {
            parameters:
                @display("p=150,100");
        }
        cryptoRemover: CryptoRemover {
            parameters:
                @display("p=400,300");
        }
        delayer: ProcessingDelayLayer {
            @display("p=285,70");
        }
    connections allowunconnected:
        delayer.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        cryptoRemover.out --> delayer.lowerLayerIn;
        lowerLayerIn --> { @display("m=s"); } --> cryptoRemover.in;

        cryptoAdder.out --> { @display("m=s"); } --> lowerLayerOut;
        delayer.lowerLayerOut --> cryptoAdder.in;
        upperLayerIn --> { @display("m=n"); } --> delayer.upperLayerIn;


}
