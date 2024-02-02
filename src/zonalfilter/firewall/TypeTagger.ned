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

package zonalfilter.firewall;

import inet.queueing.base.PacketMarkerBase;
import inet.queueing.contract.IPacketMarker;

//
// TODO auto-generated module
//
simple TypeTagger extends PacketMarkerBase like IPacketMarker
{
    parameters:
        // Would be more realistic if this was an int ID, but we're using tags anyway 
        // so it's all metadata.
        //
        // In reality, we'd attach a byte or two with an int ID to the packet and use that
        // as the type. For simulation purposes, we do this so it's easier for us to see
        // the types and just tack on an extra byte or two to simulate the overhead
        // of the ID itself. 
        string type;
        @display("i=block/star");
        @class(TypeTagger);
}
