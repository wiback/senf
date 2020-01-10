//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief TimerDispatcher public header */

#ifndef HH_SENF_Scheduler_TimerEvent_
#define HH_SENF_Scheduler_TimerEvent_ 1

// Custom includes
#include <signal.h>
#include <boost/intrusive/set_hook.hpp>
#include "ClockService.hh"
#include "FIFORunner.hh"
#include <senf/Utils/Logger/SenfLog.hh>
#include <senf/Scheduler/ClockService.hh>

//#include "TimerEvent.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {

    namespace detail {
        struct TimerSetTag;
        typedef boost::intrusive::set_base_hook< boost::intrusive::tag<TimerSetTag> > TimerSetBase;
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
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

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

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Disable timer
        void enable();                  ///< Enable timer

        void action(Callback const & cb); ///< Change timer event callback
        void timeout(ClockService::clock_type const & timeout, bool initiallyEnabled=true);
                                        ///< Re-arm or move timeout
                                        /**< \param[in] timeout new timeout time
                                             \param[in] initiallyEnabled if set \c false, do not
                                                 enable callback automatically. */
        ClockService::clock_type const & timeout() const;
                                        ///< Get current/last timeout value

    private:
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;

        Callback cb_;
        ClockService::clock_type timeout_;

        friend class detail::TimerDispatcher;
        friend struct detail::TimerSetCompare;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
