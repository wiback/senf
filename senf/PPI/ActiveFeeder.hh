// $Id$
//
// Copyright (C) 2007
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief ActiveFeeder public header */

#ifndef HH_SENF_PPI_ActiveFeeder_
#define HH_SENF_PPI_ActiveFeeder_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"
#include "IdleEvent.hh"

//#include "ActiveFeeder.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Adapter to connect passive connectors

        ActiveFeeder is an adapter module which allows two connect to passive connectors. As long as
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ActiveFeeder.cci"
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
