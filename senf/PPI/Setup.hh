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
    \brief Setup public header */

#ifndef HH_SENF_PPI_Setup_
#define HH_SENF_PPI_Setup_ 1

// Custom includes
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include "predecl.hh"

//#include "Setup.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

namespace detail {
    struct DisableStandardInput {};
    struct DisableStandardOutput {};
    struct DisableStandardConnect : public DisableStandardInput, public DisableStandardOutput {};
}

#ifdef DOXYGEN

    /** \brief Connect modules

        senf::ppi::connect() establishes a connection between two modules or, to be more precise,
        between two connectors. It will connect any input to any output connector as long as one is
        active and the other passive.

        If a module has an output connector called \c output, the module may be directly specified
        as \a source argument. In the same way, if a module has an input connector called \c input,
        the module may be given directly as \a target argument. This simplifies the most common case
        of a module with one input and one output connector.

        The connect call will check at runtime, whether the two connectors are type-compatible:
        \li Either or both of the connectors are untyped (they accept/send arbitrary senf::Packet's)
        \li Both connectors send/accept the exactly same packet type.

        Depending on the type of input or output, the connect call may require additional
        arguments. See the respective module documentation for more information

        \throws connector::IncompatibleConnectorsException if the two connectors are not type
            compatible.

        \see \ref ppi_connections
     */
    void connect(connector::OutputConnector & source, connector::InputConnector & target, ...);

#else

    void connect(connector::GenericActiveOutput & source, connector::GenericPassiveInput & target);
    void connect(connector::GenericPassiveOutput & source, connector::GenericActiveInput & target);

#endif

#ifndef DOXYGEN

    template <class T, class C>
    void connect(T & source, C & target,
                 typename boost::disable_if< boost::is_base_of<connector::Connector, T> >::type * = 0,
                 typename boost::enable_if< boost::is_base_of<connector::Connector, C> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<detail::DisableStandardOutput, T> >::type * = 0);

    template <class C, class T>
    void connect(C & source, T & target,
                 typename boost::enable_if< boost::is_base_of<connector::Connector, C> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<connector::Connector,T> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<detail::DisableStandardInput, T> >::type * = 0);

    template <class T1, class T2>
    void connect(T1 & source, T2 & target,
                 typename boost::disable_if< boost::is_base_of<connector::Connector, T1> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<connector::Connector, T2> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<detail::DisableStandardOutput, T1> >:: type * = 0,
                 typename boost::disable_if< boost::is_base_of<detail::DisableStandardInput, T2> >:: type * = 0);

#endif

    /** \brief Start the network

        Calling senf::ppi::run() will start processing the network. The main event loop is managed
        by the Scheduler. Before starting the Scheduler main loop, all Module init() members are
        called.

        senf::ppi::run() will return when no more work is to be done, that is when no events are
        enabled (Since the events are enabled and disabled by the throttle notifications which
        depend among other things on the packet queues, this is the same as checking for packets in
        any queue). It is Ok to call senf::ppi::run() multiple times during the program lifetime.

        \see \ref ppi_run
     */
    void run();

    /** \brief Manually initialize the network

        For debugging purposes, it is sometimes simpler to not use senf::ppi::run() but instead
        drive the network via explicit calls using the debug modules. However, it is still necessary
        to initialize the network. This operation is performed by senf::ppi::init().
     */
    void init();

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Setup.cci"
//#include "Setup.ct"
#include "Setup.cti"
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
