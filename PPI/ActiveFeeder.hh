// $Id$
//
// Copyright (C) 2007 
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
    \brief ActiveFeeder public header */

#ifndef HH_ActiveFeeder_
#define HH_ActiveFeeder_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"
#include "IdleEvent.hh"

//#include "ActiveFeeder.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Adapter to connect passive connectors

        ActiveFeeder is an adapter module which allows to connect to passive connectors. As long as
        none of the connectors is throttled, the ActiveFeeder will forward packets from it's input
        to it's output.

        \note For this Module to work correctly, it is very important for the connectors to be
            correctly throttled. Otherwise the system might well hang in an endless loop.

        \ingroup adapter_modules
     */
    class ActiveFeeder
        : public Module
    {
        SENF_PPI_MODULE(ActiveFeeder);
    public:
        connector::ActiveInput<> input;
        connector::ActiveOutput<> output;

        ActiveFeeder();

    private:
        void request();

        IdleEvent idle_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "ActiveFeeder.cci"
//#include "ActiveFeeder.ct"
//#include "ActiveFeeder.cti"
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
