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

import inet.node.tsn.TsnSwitch;

//
// This module represents a Firewall Zonal Gateway, which is just a TsnSwitch with
// the added features from our project.
//
// Specifically, this module checks all messages coming from one of its ECU ports to make sure 
// that the message type is allowed to be sent by that ECU. 
//
// It also checks all messages going toward one of its ECU ports to make sure that the message
// is allowed to be received by that ECU.
//
module FirewallZonalGateway extends TsnSwitch
{
    
}
