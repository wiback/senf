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
    \brief TimerDispatcher public header */

#ifndef HH_SENF_Scheduler_TimerEvent_
#define HH_SENF_Scheduler_TimerEvent_ 1

// Custom includes
#include <signal.h>
#include <senf/boost_intrusive/iset_hook.hpp>
#include "ClockService.hh"
#include "FIFORunner.hh"
#include <senf/Utils/Logger/SenfLog.hh>

//#include "TimerEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    namespace detail {
        struct TimerSetTag;
        typedef boost::intrusive::iset_base_hook<TimerSetTag> TimerSetBase;
        struct TimerSetCompare;
        class TimerDispatcher;
    }

    /** \brief Deadline timer event

        The TimerEvent class registers a deadline timer callback which will be called when the
        timer expires.

        Timer events are implemented using POSIX timers. Depending on kernel features, the timer
        resolution will be far more precise than the linux clock tick resolution. The nominal timer
        resolution is 1 nanosecond.

        The timeout time is set as \e absolute time as returned by the senf::ClockService. After
        expiration, the timer will be disabled. It may be re-enabled by setting a new timeout time.
        It is also possible to change a running timer resetting the timeout time.

        The TimerEvent class is an implementation of the RAII idiom: The event will be automatically
        unregistered in the TimerEvent destructor. The TimerEvent instance should be created
        within the same scope or on a scope below where the callback is defined (e.g. if the
        callback is a member function it should be defined as a class member).
     */
    class TimerEvent
        : public detail::FIFORunner::TaskInfo,
          public detail::TimerSetBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        TimerEvent(std::string const & name, Callback const & cb, ClockService::clock_type timeout,
                   bool initiallyEnabled = true);
                                        ///< Register a timer event
                                        /**< Registers \a cb to be called as soon as possible after
                                             the time \a timeout is reached. If \a initiallyEnabled
                                             is set \c false, the callback will not be enabled
                                             automatically. Use enable() to do so.
                                             \param[in] name Descriptive timer name (purely
                                                 informational)
                                             \param[in] cb Callback to call
                                             \param[in] timeout timeout time after the timer
                                                 will be disabled
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically. */
        TimerEvent(std::string const & name, Callback const & cb);
                                        ///< Create a timer event
                                        /**< Creates a timer event for callback \a cb. The timer is
                                             initially disabled. Use the timeout() member to set the
                                             timeout time.
                                             \param[in] name Descriptive timer name (purely
                                                 informational)
                                             \param[in] cb Callback to call. */
        ~TimerEvent();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Disable timer
        void enable();                  ///< Enable timer

        void action(Callback const & cb); ///< Change timer event callback
        void timeout(ClockService::clock_type timeout, bool initiallyEnabled=true);
                                        ///< Re-arm or move timeout
                                        /**< \param[in] timeout new timeout time
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically. */
        ClockService::clock_type timeout() const;
                                        ///< Get current/last timeout value

    private:
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;

        Callback cb_;
        ClockService::clock_type timeout_;

        friend class detail::TimerDispatcher;
        friend class detail::TimerSetCompare;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "TimerEvent.cci"
//#include "TimerEvent.ct"
//#include "TimerEvent.cti"
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
