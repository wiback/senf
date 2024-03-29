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
    template <class PacketType = Packet>
    class PassiveJoin
        : public Module,
          public MultiConnectorMixin<PassiveJoin<PacketType>, connector::PassiveInput<PacketType> >
    {
        SENF_PPI_MODULE(PassiveJoin);
    public:
        connector::ActiveOutput<PacketType> output;

        PassiveJoin();

    private:
        void connectorSetup(connector::PassiveInput<PacketType> & conn);
        void request(connector::PassiveInput<PacketType>  & input);
        void onThrottle();
        void onUnthrottle();

        friend class MultiConnectorMixin<PassiveJoin<PacketType>, connector::PassiveInput<PacketType> >;
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
//#include "Joins.cci"
#include "Joins.ct"
#include "Joins.cti"
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
