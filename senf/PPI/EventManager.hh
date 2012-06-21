// $Id$
//
// Copyright (C) 2007
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
    \brief EventManager public header */

#ifndef HH_SENF_PPI_EventManager_
#define HH_SENF_PPI_EventManager_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <senf/Scheduler/ClockService.hh>
#include "predecl.hh"
#include "detail/Callback.hh"
#include "detail/EventBinding.hh"

//#include "EventManager.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Event registry and control

        The EventManager control event registration and manages global event parameters. The
        EventManager controls event dispatch but does \e not control event generation. This is the
        responsibility of an external component (the Scheduler)
      */
    class EventManager
        : boost::noncopyable
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

#ifndef DOXYGEN
        // Somehow doxygen barfs on this definition
        template <class Descriptor>
        struct Callback
            : public detail::Callback<typename Descriptor::EventArg>
        {};
#endif

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        static EventManager & instance();
        static bool alive();

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        ClockService::clock_type now(); ///< Current time at last event dispatch
        ClockService::clock_type time(); ///< Expected time of the last event

    protected:

    private:
        EventManager();
        ~EventManager();

        template <class Descriptor>
        void registerEvent(module::Module & module,
                           typename Callback<Descriptor>::type callback,
                           Descriptor & descriptor);

        void destroyModule(module::Module & module);
        void destroyEvent(EventDescriptor & event);

        typedef boost::ptr_vector<detail::EventBindingBase> EventRegistrations;
        EventRegistrations registrations_;

        void eventTime(ClockService::clock_type time);

        ClockService::clock_type eventTime_;

        static bool alive_;

        friend class detail::EventBindingBase;
        friend class module::Module;
        friend class EventDescriptor;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
