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
