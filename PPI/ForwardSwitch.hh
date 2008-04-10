// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef FORWARDSWITCH_HH_
#define FORWARDSWITCH_HH_

// Custom includes
#include "Connectors.hh"
#include "Module.hh"

namespace senf {
namespace ppi {
namespace module {

class ForwardSwitch
    : public Module
{
    SENF_PPI_MODULE(ForwardSwitch);
public:

    connector::PassiveInput<> input;
    connector::ActiveOutput<> output;

    ForwardSwitch(bool state);
    bool forward();
    bool forward(bool state);
    bool stopForwarding();
    bool startForwarding();
private:
    void onRequest();	
	bool forward_;

};

}}} //namespaces
#include "ForwardSwitch.cci"
#endif /*FORWARDSWITCH_HH_*/
