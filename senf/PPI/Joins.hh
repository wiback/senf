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
    \brief Joins public header */

#ifndef HH_SENF_PPI_Joins_
#define HH_SENF_PPI_Joins_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"
#include "MultiConnectorMixin.hh"

//#include "Joins.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

namespace module {

    /** \brief Join multiple packet streams with passive inputs

        The PassiveJoin will combine any number of packet streams. You may connect any number of
        ActiveOutput<>'s  to the PassiveJoin instance. The combined stream is then provided on the
        ActiveOutput<> \a output.

        Since PassiveJoin allows any number of incoming packet streams, the input connectors are
        dynamically managed. A special senf::ppi::connect() overload is used to dynamically create
        the needed input connectors. This hides this extra functionality from the user.
        \code
        senf::ppi::module::PassiveJoin join;

        ppi::connect(module1,join);             // Connect first module to join's input
        ppi::connect(module2.some_output,join); // Connect another module to join's input
        ppi::connect(join,module3);             // Forward combined stream to module3
        \endcode

        \ingroup routing_modules
     */
    class PassiveJoin
        : public Module,
          public MultiConnectorMixin<PassiveJoin, connector::PassiveInput<> >
    {
        SENF_PPI_MODULE(PassiveJoin);
    public:
        connector::ActiveOutput<> output;

        PassiveJoin();

    private:
        void connectorSetup(connector::PassiveInput<> & conn);
        void request(connector::GenericPassiveInput & input);
        void onThrottle();
        void onUnthrottle();

        friend class MultiConnectorMixin<PassiveJoin, connector::PassiveInput<> >;
    };

    /** \brief Join multiple packet streams with active inputs

        The PriorityJoin will combine any number of packet streams. You may connect any number of
        PassiveInput<>'s  to the PassiveJoin instance. The combined stream is then provided on the
        PassiveOutput<> \a output.

        When a packet request is received on Priorityjoin's \a output, The request will be serviced
        from the first unthrottled input. The order, in which connectors are connected to the
        PriorityJoin's input is important: The earlier connected peer has the higher priority and
        will be serviced first.

        Since PriorityJoin allows any number of incoming packet streams, the input connectors are
        dynamically managed. A special senf::ppi::connect() overload is used to dynamically create
        the needed input connectors. This hides this extra functionality from the user.
        \code
        senf::ppi::module::PriorityJoin join;

        ppi::connect(module1,join);             // Connect first module to join's input
        ppi::connect(module2.some_output,join); // Connect another module to join's input
        ppi::connect(join,module3);             // Forward combined stream to module3
        \endcode
        Here, \a module1 has higher priority than \a module2 which will only be queried if \a
        module1 is throttled.

        \ingroup routing_modules
     */
    class PriorityJoin
        : public Module,
          public MultiConnectorMixin<PriorityJoin, connector::ActiveInput<> >
    {
        SENF_PPI_MODULE(PriorityJoin);
    public:
        connector::PassiveOutput<> output;

        PriorityJoin();

    private:
        void connectorSetup(PriorityJoin::ConnectorType & conn, int priority=-1);
        void request();
        void onThrottle();
        void onUnthrottle();

        friend class MultiConnectorMixin<PriorityJoin, connector::ActiveInput<> >;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Joins.cci"
//#include "Joins.ct"
//#include "Joins.cti"
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
