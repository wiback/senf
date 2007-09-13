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
        between two connectors. It will connect any input to any output connector as long as one is
        active and the other passive.

        If a module has an output connector called \c output, the module may be directly specified
        as \a source argument. In the same way, if a module has an input connector called \c input,
        the module may be given directly as \a target argument. This simplifies the most common case
        of a module with one input and one output connector.
        
        \see \ref ppi_connections
     */
    void connect(connector::ActiveInput & source, connector::PassiveOutput & target);

    /** \brief Connect modules
        \see connect() */
    void connect(connector::PassiveInput & source, connector::ActiveOutput & target);

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
