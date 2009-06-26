// $Id$
//
// Copyright (C) 2008 
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
    \brief AnnotationRouter public header */

#ifndef HH_SENF_PPI_AnnotationRouter_
#define HH_SENF_PPI_AnnotationRouter_ 1

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include "../Utils/String.hh"
#include "Module.hh"
#include "Connectors.hh"
#include "DynamicConnectorMixin.hh"

//#include "AnnotationRouter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        \li Copy construction and copy assignment (\c Copyable und \c Assignable concepts)
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
          public DynamicConnectorMixin< AnnotationRouter<AnnotationType>,
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
        
        typedef boost::ptr_map<AnnotationType, connector::ActiveOutput<> > Outputs;
        Outputs outputs_;

        friend class DynamicConnectorMixin< AnnotationRouter<AnnotationType>,
                                            connector::ActiveOutput<>,
                                            AnnotationType >;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
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
