//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Duplicators public header */

#ifndef HH_SENF_PPI_Duplicators_
#define HH_SENF_PPI_Duplicators_ 1

// Custom includes
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
//#include "Duplicators.cci"
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
