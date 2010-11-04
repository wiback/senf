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
    \brief CloneSource public header */

#ifndef HH_SENF_PPI_CloneSource_
#define HH_SENF_PPI_CloneSource_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
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
