// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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

/** \file
    \brief psi2ts.hh public header */

#ifndef HH_psi2ts_
#define HH_psi2ts_ 1

// Custom includes
#include <senf/PPI/PPI.hh>
#include <senf/PPI/Module.hh>
#include <senf/PPI/Connectors.hh>
#include <senf/Packets/PacketData.hh>
#include <senf/Packets/MPEGDVBBundle/TransportPacket.hh>

//#include "psi2ts.mpp"
///////////////////////////////hh.p////////////////////////////////////////

class Psi2TsModule
    : public senf::ppi::module::Module
{
    SENF_PPI_MODULE(Psi2TsModule);

public: 
    senf::ppi::connector::PassiveInput<> input;
    senf::ppi::connector::ActiveOutput<senf::TransportPacket> output;
    Psi2TsModule(); 
    void onRequest();
    
private:
    enum state {IDLE, PROC, WAIT};
    typedef senf::PacketData::iterator iterator;
    unsigned continuity_counter_;
    unsigned next_continuity_counter();
    state state_;
        
};


///////////////////////////////hh.e////////////////////////////////////////
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
