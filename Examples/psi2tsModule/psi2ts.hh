// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief psi2ts.hh public header */

#ifndef HH_SENF_Examples_psi2tsModule_psi2ts_
#define HH_SENF_Examples_psi2tsModule_psi2ts_ 1

// Custom includes
#include <senf/PPI/PPI.hh>
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include <senf/Packets/PacketData.hh>
#include <senf/Packets/MPEGDVBBundle/TransportPacket.hh>

//#include "psi2ts.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

class Psi2TsModule
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Psi2TsModule);

public:
    senf::ppi::connector::PassiveInput<> input;
    senf::ppi::connector::ActiveOutput<senf::TransportPacket> output;
    Psi2TsModule(unsigned pid, senf::ClockService::clock_type timout=0);
    void onRequest();

private:
    enum state {IDLE, PROC, WAIT};
    typedef senf::PacketData::iterator iterator;
    unsigned continuity_counter_;
    unsigned next_continuity_counter();
    state state_;
    unsigned pid_;
    senf::ClockService::clock_type timeout_;
};


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "psi2ts.cci"
//#include "psi2ts.ct"
//#include "psi2ts.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
