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
    \brief CloneSource public header */

#ifndef HH_SENF_PPI_CloneSource_
#define HH_SENF_PPI_CloneSource_ 1

// Custom includes
#include "Module.hh"
#include "Connectors.hh"

//#include "CloneSource.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Generate clone's of a template packet

        CloneSource will provide clone's of a template \a packet on it's \a output.

        \ingroup io_modules
     */
    class CloneSource
        : public Module
    {
        SENF_PPI_MODULE(CloneSource);
    public:

        connector::PassiveOutput<> output;

        CloneSource(senf::Packet const & packet);
        void replacePacket(senf::Packet const & packet);

    private:
        void request();

        senf::Packet packet_;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "CloneSource.cci"
//#include "CloneSource.ct"
//#include "CloneSource.cti"
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
