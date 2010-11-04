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
    \brief Duplicators public header */

#ifndef HH_SENF_PPI_Duplicators_
#define HH_SENF_PPI_Duplicators_ 1

// Custom includes
#include <boost/ptr_container/ptr_vector.hpp>
#include "predecl.hh"
#include "Connectors.hh"
#include "Module.hh"
#include "MultiConnectorMixin.hh"

//#include "Duplicators.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Copy every incoming packet to each output

        ActiveDuplicator will take every received packet and push it out to each connected output.

        Since ActiveDuplicator allows any number of incoming packet streams, the input connectors
        are dynamically managed. A special senf::ppi::connect() overload is used to dynamically
        create the needed input connectors. This hides this extra functionality from the user.
        \code
        senf::ppi::module::ActiveDuplicator dup;

        senf::ppi::connect(sourceModule, dup);
        senf::ppi::connect(dup, targetModule1);
        senf::ppi::connect(dup, targetModule2.some_input);
        \endcode

        \ingroup routing_modules
     */
    class ActiveDuplicator
        : public Module,
          public MultiConnectorMixin<ActiveDuplicator, connector::ActiveOutput<> >
    {
        SENF_PPI_MODULE(ActiveDuplicator);
    public:
        connector::PassiveInput<> input;

        ActiveDuplicator();

    private:
        void connectorSetup(ActiveDuplicator::ConnectorType & conn);
        void request();

        friend class MultiConnectorMixin<ActiveDuplicator, connector::ActiveOutput<> >;
    };

}}}



//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Duplicators.cci"
//#include "Duplicators.ct"
//#include "Duplicators.cti"
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
