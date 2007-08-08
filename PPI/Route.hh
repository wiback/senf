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
    \brief Route public header */

#ifndef HH_Route_
#define HH_Route_ 1

// Custom includes
#include <boost/type_traits.hpp>
#include "predecl.hh"

//#include "Route.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    class RouteBase
    {
    public:
        void autoThrottling(bool state); ///< Change automatic throttle notification forwarding
                                        /**< By default, throttle notifications are automatically
                                             forwarded from active to passive connectors. This may
                                             be disabled by setting the authoThrottling state to \c
                                             false.
                                             
                                             Routing from/to an event to/from a passive connector
                                             will automatically create throttling notifications on
                                             the connector whenever the event is disabled. Routing
                                             form/to an event to/from an active connector will
                                             disable the event whenever a throttling notification
                                             comes in. Respective for unthrottle notifications.

                                             \param[in] state New throttle forwarding state 

                                             \implementation This class will be implemented using a
                                                 baseclass, this template and several
                                                 specializations. However, this is an implementation
                                                 detail which does not affect the exposed
                                                 interface. */

    protected:
        RouteBase(module::Module & module);

    private:
        module::Module * module_;
    };

}}

#include "Route.ih"

namespace senf {
namespace ppi {

    /** \brief Route descriptor
        
        Route instances are created by Module::route statements. The Route class provides an
        interface to manipulate the flow processing.
     */
    template <class Source, class Target>
    class Route
        : public detail::RouteImplementation< boost::is_base_of<EventDescriptor,Source>::value,
                                              boost::is_base_of<EventDescriptor,Target>::value >
    {
    private:
        typedef detail::RouteImplementation< 
            boost::is_base_of<EventDescriptor,Source>::value,
            boost::is_base_of<EventDescriptor,Target>::value > Implementation;
        
        Route(module::Module & module, Source & source, Target & target);

        friend class module::Module;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Route.cci"
//#include "Route.ct"
#include "Route.cti"
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
