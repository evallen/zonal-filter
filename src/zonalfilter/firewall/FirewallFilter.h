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

#ifndef __ZONALFILTER_FIREWALLFILTER_H_
#define __ZONALFILTER_FIREWALLFILTER_H_

#include "inet/common/packet/PacketFilter.h"
#include "inet/queueing/base/PacketFilterBase.h"
#include "inet/common/ModuleRefByPar.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

using namespace inet::queueing;
using namespace inet;

class FirewallFilter : public PacketFilterBase, public TransparentProtocolRegistrationListener
{
  protected:
    ModuleRefByPar<IInterfaceTable> interfaceTable;
    cValueMap *rules = nullptr;

  private:
    bool _checkRules(const char *interfaceName, const char *type, const char *inoutkey) const;
    bool checkRulesIn(const char *interfaceName, const char *type) const;
    bool checkRulesOut(const char *interfaceName, const char *type) const;
    bool checkRules(const char *interfaceName, const char *type) const;

    const char * getInterfaceName(const Packet * packet) const;

  protected:
    virtual void initialize(int stage) override;

    virtual cGate *getRegistrationForwardingGate(cGate *gate) override;

    virtual bool matchesPacket(const Packet *packet) const override;

};

#endif
