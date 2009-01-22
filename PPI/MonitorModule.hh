// $Id$
//
// Copyright (C) 2009 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief MonitorModule public header */

#ifndef HH_SENF_PPI_MonitorModule_
#define HH_SENF_PPI_MonitorModule_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"

//#include "MonitorModule.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    class MonitorModule : public Module
    {
    public:
        senf::ppi::connector::PassiveInput<> input;
        senf::ppi::connector::ActiveOutput<> output;

    protected:
        MonitorModule();

        virtual void v_handlePacket(Packet const & p) = 0;

    private:
        void request();
        void throttle();
        void unthrottle();
    };
        

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "MonitorModule.cci"
//#include "MonitorModule.ct"
//#include "MonitorModule.cti"
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
