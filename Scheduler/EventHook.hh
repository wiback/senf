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
    \brief EventHook public header */

#ifndef HH_SENF_Scheduler_EventHook_
#define HH_SENF_Scheduler_EventHook_ 1

// Custom includes
#include <boost/function.hpp>
#include "../boost/intrusive/ilist_hook.hpp"
#include "FIFORunner.hh"

//#include "EventHook.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    namespace detail {
        struct EventHookListTag;
        typedef boost::intrusive::ilist_base_hook<EventHookListTag> EventHookListBase;
        class EventHookDispatcher;
    }

    /** \brief Event hook event

        This event is special: It is not a real event, it is a kind of hook which is called,
        whenever any other event is signaled. Combining this with explicit priority specification,
        this can be used to implement hooks which are called before or after any other callback.

        \code
        void beforeEventHook();
        void afterEventHook();

        senf::scheduler::EventHook beforeEventHookEvent (
            "beforeEventHook", beforeEventHook, true, senf::scheduler::EventHook::POST);
        senf::scheduler::EventHook afterEventHookEvent (
            "afterEventHook", afterEventHook, true, senf::scheduler::EventHook::PRE);
        \endcode

        The EventHook class is an implementation of the RAII idiom: The event will be automatically
        unregistered in the EventHook destructor. The EventHook instance should be created within
        the same scope or on a scope below where the callback is defined (e.g. if the callback is a
        member function it should be defined as a class member).
      */
    class EventHook
        : public detail::FIFORunner::TaskInfo,
          public detail::EventHookListBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        static Priority const PRE = PRIORITY_HIGH; ///< Execute hook BEFORE all other events
        static Priority const POST = PRIORITY_LOW; ///< Execute hook AFTER all other events

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        EventHook(std::string const & name, Callback const & cb,
                   bool initiallyEnabled = true, Priority priority = POST);
                                        ///< Register an event hook
                                        /**< Registers \a cb to be called whenever any other event
                                             is signaled by the scheduler. If \a initiallyEnabled is
                                             set \c false, the callback will not be enabled
                                             automatically. Use enable() to do so.
                                             \param[in] name Descriptive event name (purely
                                                 informational)
                                             \param[in] cb Callback to call
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically. 
                                             \param[in] priority event priority, defaults to
                                                 POST */
        ~EventHook();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Disable event
        void enable();                  ///< Enable event

        void action(Callback const & cb); ///< Change event callback

    protected:

    private:
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;

        Callback cb_;

        friend class detail::EventHookDispatcher;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "EventHook.cci"
//#include "EventHook.ct"
//#include "EventHook.cti"
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
