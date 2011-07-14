// $Id$
//
// Copyright (C) 2008
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
    \brief AnnotationRouter public header */

#ifndef HH_SENF_PPI_AnnotationRouter_
#define HH_SENF_PPI_AnnotationRouter_ 1

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include <senf/Utils/String.hh>
#include "Module.hh"
#include "Connectors.hh"
#include "MultiConnectorMixin.hh"

//#include "AnnotationRouter.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief %Route packets to destination according to some annotation value

        This router takes packet on a single input and directs them to one of it outputs depending
        on a \ref packet_usage_annotation "packet annotation". Each output connected
        will be associated with a single annotation value. Incoming packets for which no matching
        output is found are directed to a default output. If this output is left unconnected, those
        packets will be dropped.

        The \a AnnotationType template parameter defines the routing key. This annotation must
        support the following operations:
        \li Comparison with '<' (\c LessThanComparable concept)
        \li Copy construction and copy assignment (\c Copyable and \c Assignable concepts)
            (e.g. via compiler synthesized copy constructor and assignment operator)
        \li Output streaming to an ostream via '\c <<' (for error description purposes) (\c
            OutputStreamable concept)

        The following annotation can be used to route the packets according to a mac address.
        \code
        struct TargetInterface
        {
            senf::MACAddress mac;

            bool operator< (TargetInterface const & other)
                { return mac < other.mac; }

            TargetInterface(senf::MACAddress const & m)
                : mac (m) {}
        };

        std::ostream & operator<<(std::ostream & os, TargetInterface const & v)
            { os << v.mac; return os; }
        \endcode

        The additional senf::MACAddress constructor allows to construct an instance directly from a
        mac address and allows to pass a senf::MACAddress value as routing key directly:

        \code
        senf::ppi::module::AnnotationRouter<TargetInterface> router;

        senf::ppi::connect(router, target1, senf::MACAddress::from_string("00:1a:2b:04:06:08"));
        \endcode

        The special senf::ppi::connect() overload takes a third argument, the routing key. This must
        be an AnnotationType value or must be (explicitly) convertible to AnnotationType.

        The input will be throttled whenever any of the outputs except \a defaultOutput are
        throttled.

        \ingroup routing_modules

        \todo Call Module::v_init() on every connection change and remove disconnected connections
        from the container
     */
    template <class AnnotationType>
    class AnnotationRouter
        : public Module,
          public MultiConnectorMixin< AnnotationRouter<AnnotationType>,
                                        connector::ActiveOutput<>,
                                        AnnotationType >
    {
        SENF_PPI_MODULE(AnnotationRouter);
    public:
        connector::PassiveInput<> input;
        connector::ActiveOutput<> defaultOutput;

        AnnotationRouter();

        struct DuplicateKeyException : public senf::Exception
        { DuplicateKeyException(AnnotationType const & key)
              : senf::Exception("Duplicate senf::ppi::module::AnnotationRouter routing key ")
                { append( senf::str(key)); } };

    private:
        AnnotationType connectorSetup(connector::ActiveOutput<> & conn, AnnotationType const & key);
        void request();

        friend class MultiConnectorMixin< AnnotationRouter<AnnotationType>,
                                            connector::ActiveOutput<>,
                                            AnnotationType >;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "AnnotationRouter.cci"
#include "AnnotationRouter.ct"
//#include "AnnotationRouter.cti"
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
