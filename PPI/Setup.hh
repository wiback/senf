// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Setup public header */

#ifndef HH_Setup_
#define HH_Setup_ 1

// Custom includes
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include "predecl.hh"

//#include "Setup.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

#ifndef DOXYGEN    

    void connect(connector::ActiveOutput & source, connector::PassiveInput & target);
    void connect(connector::PassiveOutput & source, connector::ActiveInput & target);
    
    template <class T, class C>
    void connect(T & source, C & target,
                 typename boost::disable_if< boost::is_base_of<connector::Connector, T> >::type * = 0,
                 typename boost::enable_if< boost::is_base_of<connector::Connector, C> >::type * = 0);

    template <class C, class T>
    void connect(C & source, T & target,
                 typename boost::enable_if< boost::is_base_of<connector::Connector, C> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<connector::Connector,T> >::type * = 0);

    template <class T1, class T2>
    void connect(T1 & source, T2 & target,
                 typename boost::disable_if< boost::is_base_of<connector::Connector, T1> >::type * = 0,
                 typename boost::disable_if< boost::is_base_of<connector::Connector, T2> >::type * = 0);

#else

    /** \brief Connect modules

        senf::ppi::connect() establishes a connection between two modules or, to be more precise,
        between two connectors. For enhanced usability, \a source and \a target may be a Connector,
        a Module or a collection/subnetwork. Passing a Module or collection/subnetwork as \a source
        will originate the connection on the \c output member of that Module or collection while
        passing a module or collection/subnetwork as \a target will terminate the connection on that
        Module or collections \c input member. For most simple modules, the specification of the
        connector is therefore obsolete.

        Furthermore, the connect() call may be extended by special modules (e.g. PassiveJoin which
        allows an arbitrary of input connections).
     */
    template <class Source, class Target>
    void connect(Source & source, Target & target);

#endif
    
    /** \brief Start the network

        Calling senf::ppi::run() will start processing the network. The main event loop is managed
        by the Scheduler. Before starting the Scheduler main loop, all Module init() members are
        called.

        senf::ppi::run() will return when no more work is to be done, that is when no events are
        enabled (Since the events are enabled and disabled by the throttle notifications which
        depend among other things on the packet queues, this is the same as checking for packets in
        any queue). It is Ok to call senf::ppi::run() multiple times during the program lifetime.
     */
    void run();

    /** \brief Manually initialize the network
        
        For debugging purposes, it is sometimes simpler to not use senf::ppi::run() but instead
        drive the network via explicit calls using the debug modules. However, it is still necessary
        to initialize the network. This operation is performed by senf::ppi::init().
     */
    void init();

}}

///////////////////////////////hh.e////////////////////////////////////////
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
