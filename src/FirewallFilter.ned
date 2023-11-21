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

import inet.queueing.base.PacketFilterBase;
import inet.queueing.contract.IPacketFilter;

//
// TODO auto-generated module
//
simple FirewallFilter extends PacketFilterBase like IPacketFilter
{
    parameters:
        string interfaceTableModule;
        
        //
        // Set of rules for each enforced interface.
        // Decides which message types are allowed to enter from an interface ('out')
        // or exit into an interface ('in'). 'out'/'in' conventions are named so to
        // reflect that the messages are going 'out of' or 'into' the associated ECUs
        // on each port.
        //
        // Should take the form of a dictionary like so:
        // { <interface>: { in: [ <MSGTYPE>,... ], out: [ <MSGTYPE>,... ] }, ... }
        //
        // Example (suppose eth0 has a camera, eth1 has a wheel, eth2 is to other switches):
        // { 
        // 		'eth0': { in: [ ], out: [ 'FL_CAM_IMAGE' ] }, 
        //		'eth1': { in: [ 'FL_WHEEL_CONTROL' ], out: [ ] }
        // }
        // (^ 'eth2' intentionally omitted since we aren't enforcing a rule for it)
        //
        object rules;
        
        // True if this module is an ingress filter, and false if it is an egress filter.
        bool isIngress;
        @class(FirewallFilter);
}
