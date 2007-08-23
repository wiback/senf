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
    \brief EventManager public header */

#ifndef HH_EventManager_
#define HH_EventManager_ 1

// Custom includes
#include <boost/ptr_container/ptr_vector.hpp>
#include "Scheduler/ClockService.hh"
#include "predecl.hh"
#include "detail/Callback.hh"
#include "detail/EventBinding.hh"

//#include "EventManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Event registry and control

        The EventManager control event registration and manages global event parameters. The
        EventManager controls event dispatch but does \e not control event generation. This is the
        responsibility of an external component (the Scheduler)
      */
    class EventManager
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

#ifndef DOXYGEN
        // Somehow doxygen barfs on this definition
        template <class Descriptor>
        struct Callback
            : public detail::Callback<typename Descriptor::EventArg>
        {};
#endif

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static EventManager & instance();

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ClockService::clock_type now(); ///< Current time at last event dispatch
        ClockService::clock_type time(); ///< Expected time of the last event

    protected:

    private:
        template <class Descriptor>
        void registerEvent(module::Module & module,
                           typename Callback<Descriptor>::type callback,
                           Descriptor & descriptor);

        void destroyModule(module::Module & module);

        typedef boost::ptr_vector<detail::EventBindingBase> EventRegistrations;
        EventRegistrations registrations_;

        void eventTime(ClockService::clock_type time);

        ClockService::clock_type eventTime_;

        friend class detail::EventBindingBase;
        friend class module::Module;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "EventManager.cci"
#include "EventManager.ct"
//#include "EventManager.cti"
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
