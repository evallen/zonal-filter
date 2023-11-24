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

#include "zonalfilter/firewall/FirewallFilter.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "zonalfilter/firewall/TypeTag_m.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include <omnetpp.h>

using namespace omnetpp;

Define_Module(FirewallFilter);

void FirewallFilter::initialize(int stage)
{
    PacketFilterBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        interfaceTable.reference(this, "interfaceTableModule", true);
        rules = check_and_cast<cValueMap *>(par("rules").objectValue());
        WATCH(rules);
    }
}

cGate *FirewallFilter::getRegistrationForwardingGate(cGate *gate)
{
    if (gate == outputGate)
        return inputGate;
    else if (gate == inputGate)
        return outputGate;
    else
        throw cRuntimeError("Unknown gate");
}

// inoutkey is either "in" or "out"
bool FirewallFilter::_checkRules(const char *interfaceName, const char *type, const char *inoutkey) const
{
    if (!rules->containsKey(interfaceName)) {
        return true; // If no entry for the interface exists, assume OK (not enforced)
    }

    cValueMap *interfaceRules = check_and_cast<cValueMap *>(rules->get(interfaceName).objectValue());

    if (!interfaceRules->containsKey(inoutkey)) {
        return false; // If no entry for "out" / "in" exists, assume none allowed out
    }

    cValueArray *inoutRules = check_and_cast<cValueArray *>(interfaceRules->get(inoutkey).objectValue());

    for (int i = 0; i < inoutRules->size(); i++) {
        if (strcmp(inoutRules->get(i).stringValue(), type) == 0) {
            return true; // Found message type in in/out table.
        }
    }
    return false; // If no entry for the type exists in the in/out table, block it.
}

bool FirewallFilter::checkRulesIn(const char *interfaceName, const char *type) const
{
    return _checkRules(interfaceName, type, "in");
}

bool FirewallFilter::checkRulesOut(const char *interfaceName, const char *type) const
{
    return _checkRules(interfaceName, type, "out");
}

bool FirewallFilter::checkRules(const char *interfaceName, const char *type) const
{
    if (par("isIngress").boolValue())
    {
        // Confusing naming - here, we are checking that the message can leave
        // the given ECU ("Out"), meaning that it's ingressing into the switch.
        return checkRulesOut(interfaceName, type);
    }
    return checkRulesIn(interfaceName, type);
}

const char * FirewallFilter::getInterfaceName(const Packet *packet) const
{
    NetworkInterface * networkInterface;
    if (par("isIngress").boolValue())
    {
        auto interfaceInd = packet->findTag<InterfaceInd>();
        networkInterface = interfaceInd != nullptr ?
                interfaceTable->getInterfaceById(interfaceInd->getInterfaceId()) : nullptr;
    }
    else
    {
        auto interfaceReq = packet->findTag<InterfaceReq>();
        networkInterface = interfaceReq != nullptr ?
                interfaceTable->getInterfaceById(interfaceReq->getInterfaceId()) : nullptr;
    }

    auto interfaceName = networkInterface != nullptr ?
            networkInterface->getInterfaceName() : "";

    return interfaceName;
}

bool FirewallFilter::matchesPacket(const Packet *packet) const
{
    auto interfaceName = getInterfaceName(packet);

    if (strcmp(interfaceName, "") == 0) {
        EV_WARN << "Unknown incoming interface!";
    }

    auto typeTags = packet->getAllRegionTags<TypeTag>();
    if (typeTags.size() == 0) {
        return true; // Let through untyped traffic, which is likely from other protocols (gPTP, etc.)
                     // that we're not trying to mess with.
                     // For security purposes, we can assume ECUs would only accept typed messages
                     // for actual control reasons, etc.
    }
    auto typeTag = typeTags[0].getTag();
    auto typeStr = typeTag->getType();

    bool result = checkRules(interfaceName, typeStr);
    const char * ingressEgressStr = par("isIngress").boolValue() ? "(INGRESS)" : "(EGRESS)";
    if (result)
    {
        EV_DEBUG << ingressEgressStr << " " << "Packet ok";
    }
    else
    {
        EV_DEBUG << ingressEgressStr << " " << "PACKET BLOCKED";
    }
    return result;
}
