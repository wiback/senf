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
    \brief Module public header 
*/

#ifndef HH_Module_
#define HH_Module_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

//#include "Module.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

    /** \brief Module baseclass

        senf::ppi::Module is the baseclass of all PPI modules. It provides the module implementation
        with interfaces to several PPI facilities:
        
        \li Connector management
        \li Flow management (routing)
        \li Event handling

        To provide internal bookkeeping, most access to the PPI infrastructure is managed through
        this base class. 

        Instances of this class may be allocated either statically or dynamically. Dynamic instances
        are automatically managed using the dynamicModule adaptor.
     */
    class Module
        : boost::noncopyable
    {
    protected:
        Module();
        ~Module();

        template <class Source, class Target>
        Route<Source, Target> & route(Source const & source, Target const & target); 
                                        ///< Define flow information
                                        /**< Using the route() and noroute() members, the
                                             information flow within the module is defined. Routing
                                             may be specified either between inputs, outputs and
                                             events. The routing information is used to perform
                                             automatic throttling. The throttling behavior may
                                             however be controlled manually.

                                             Even if no automatic throttling is desired <em>it is
                                             vital to define the flow information for all inputs and
                                             outputs</em>. Without flow information important
                                             internal state of the module cannot be
                                             initialized. This includes, explicitly defining
                                             terminal inputs and outputs using noroute. Event
                                             routing however is optional.

                                             The return value may be used to alter routing
                                             parameters like throttling parameters.
                                             
                                             \param[in] source Data source, object which controls
                                                 incoming data
                                             \param[in] target Data target, object which controls
                                                 outgoing data
                                             \returns Route instance describing this route */

        template <class Connector>
        void noroute(Connector const & connector); ///< Define terminal connectors
                                        /**< The noroute() member explicitly declares, that a
                                             connector is terminal and does not directly
                                             receive/forward data from/to some other
                                             connector. <em>It is mandatory to define routing
                                             information for terminal connectors</em>.

                                             See the route() documentation for more on routing
                                             
                                             \param[in] connector Terminal connector to declare */

        template <class Target, class Descriptor>
        void registerEvent(Target target, Descriptor const & descriptor);
                                        ///< Register an external event
                                        /**< The \a target argument may be either an arbitrary
                                             callable object or it may be a member function pointer
                                             pointing to a member function of the Module derived
                                             classed. The handler may \e optionally take an Argument
                                             of type <tt>typename Descriptor::Event const
                                             &</tt>. This object allows to access detailed
                                             information on the event delivered.

                                             The \a descriptor describes the event to signal. This
                                             may be a timer event or some type of I/O event on a
                                             file descriptor or socket.

                                             \param[in] target The handler to call whenever the
                                                 event is signaled
                                             \param[in] descriptor The type of event to register */

        boost::posix_time::ptime eventTime(); ///< Return timestamp of the currently processing
                                              ///< event
    };

    /** \brief Connect compatible connectors

        connect() will connect two compatible connectors: One connector must be active, the other
        passive.
     */
    template <class Source, class Target>
    void connect(Source const & source, Target const & target);

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Module.cci"
//#include "Module.ct"
//#include "Module.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
