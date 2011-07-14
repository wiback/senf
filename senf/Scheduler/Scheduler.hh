// $Id$
//
// Copyright (C) 2006
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
    \brief Scheduler public header
 */

#ifndef HH_SENF_Scheduler_Scheduler_
#define HH_SENF_Scheduler_Scheduler_ 1

// Custom includes
#include <boost/utility.hpp>
#include <senf/Utils/Logger/TimeSource.hh>
#include "FdEvent.hh"
#include "TimerEvent.hh"
#include "SignalEvent.hh"
#include "IdleEvent.hh"
#include "EventHook.hh"

//#include "scheduler.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

/** \brief The Scheduler interface

    The %scheduler API is comprised of two parts:

    \li Specific \ref sched_objects, one for each type of event.
    \li Some <a href="#autotoc-7.">generic functions</a> implemented in the \ref senf::scheduler
        namespace.

    Events are registered via the respective event class. The (global) functions are used to enter
    the application main-loop or query for global information.

    \autotoc


    \section sched_objects Event classes

    The Scheduler is based on the RAII principle: Every event is represented by a class
    instance. The event is registered in the constructor and removed by the destructor of that
    instance. This implementation automatically links the lifetime of an event with the lifetime of
    the object responsible for it's creation.

    Every event registration is represented by an instance of an event specific class:

    \li senf::scheduler::FdEvent for file descriptor events
    \li senf::scheduler::TimerEvent for single-shot deadline timer events
    \li senf::scheduler::SignalEvent for UNIX signal events
    \li senf::scheduler::EventHook for a special event hook

    These instance are owned and managed by the user of the %scheduler \e not by the %scheduler so
    the RAII concept can be used.

    \code
    class SomeServer
    {
        SomeSocketHandle handle_;
        senf::scheduler::FdEvent event_;

    public:
        SomeServer(SomeSocketHandle handle)
            : handle_ (handle),
              event_ ("SomeServer handler", senf::membind(&SomeServer::readData, this),
                      handle, senf::scheduler::FdEvent::EV_READ)
        {}

        void readData(int events)
        {
            // read data from handle_, check for eof and so on.
        }
    };
    \endcode

    The event is defined as a class member variable. When the event member is initialized in the
    constructor, the event is automatically registered (except if the optional \a initiallyEnabled
    flag argument is set to \c false). The Destructor will automatically remove the event from the
    %scheduler and ensure, that no dead code is called accidentally.

    The process is the same for the other event types or when registering multiple events. For
    detailed information on the constructor arguments and other features see the event class
    documentation referenced below.


    \section sched_handlers Specifying handlers

    All handlers are specified as generic <a
    href="http://www.boost.org/doc/libs/release/libs/functional/index.html">Boost.Function</a>
    objects. This allows to pass any callable as a handler. Depending on the type of handler,
    some additional arguments may be passed to the handler by the %scheduler.

    If you need to pass additional information to your handler, use <a
    href="http://www.boost.org/doc/libs/release/libs/bind/bind.html">Boost.Bind</a>:
    \code
    // Handle callback function
    void callback(UDPv4ClientSocketHandle handle, senf::Scheduler::EventId event) {..}
    // Pass 'handle' as additional first argument to callback()
    senf::scheduler::FdEvent event ("name", boost::bind(&callback, handle, _1),
                                    handle, senf::scheduler::FdEvent::EV_READ);
     // Timeout function
    void timeout( int n) {..}
    // Call timeout() handler with argument 'n'
    senf::scheduler::TimerEvent timer ("name", boost::bind(&timeout, n),
                                       senf::ClockService::now() + senf::ClockService::seconds(1));
    \endcode

    To use member-functions as callbacks, use either <a
    href="http://www.boost.org/doc/libs/release/libs/bind/bind.html">Boost.Bind</a> or senf::membind()
    \code
    // e.g. in Foo::Foo() constructor:
    Foo::Foo()
        : handle_ (...),
          readevent_ ("Foo read", senf::membind(&Foo::callback, this),
                      handle_, senf::scheduler::FdEvent::EV_READ)
    { ... }
    \endcode

    The handler is identified by an arbitrary, user specified name. This name is used in error
    messages to identify the failing handler.


    \section sched_exec Executing the Scheduler

    To enter the %scheduler main-loop, call

    \code
    senf::scheduler::process();
    \endcode

    This call will only return in two cases:

    \li When a handler calls senf::scheduler::terminate()
    \li When there is no active file descriptor or timer event.

    Additional <a href="#autotoc-7.">generic functions</a> provide information and %scheduler
    parameters.

    \section sched_container Event objects and container classes

    As the event objects are \e not copyable, they cannot be placed into ordinary
    containers. However, it is quite simple to use pointer containers to hold event instances:

    \code
    #include <boost/ptr_container/ptr_map.hpp>
    #include <boost/bind.hpp>

    class Foo
    {
    public:
        void add(int fd)
        {
            fdEvents.insert(
                fd,
                new senf::scheduler::FdEvent("foo", boost::bind(&callback, this, fd, _1), fd,
                                             senf::scheduler::FdEvent::EV_READ) );
        }

        void callback(int fd, int events)
        {
            FdEvent & event (fdEvents_[fd]);

            // ...

            if (complete)
                fdEvents_.remove(fd)
        }

    private:
        boost::ptr_map<int, FdEvent> fdEvents_;
    };
    \endcode

    The pointer container API is (almost) completely identical to the corresponding standard library
    container API. The only difference is, that all elements added to the container \e must be
    created via \c new and that the pointer containers themselves are \e not copyable (ok, they are,
    if the elements are cloneable ...). See <a
    href="http://www.boost.org/doc/libs/release/libs/ptr_container/doc/ptr_container.html">Boost.PointerContainer</a>
    for the pointer container library reference.


    \section sched_signals Signals and the Watchdog

    To secure against blocking callbacks, the %scheduler implementation includes a watchdog
    timer. This timer will produce a warning message on the standard error stream when a single
    callback is executing for more than the watchdog timeout value. Since the %scheduler
    implementation is completely single threaded, we cannot terminate the callback but at least we
    can produce an informative message and optionally the program can be aborted.

    The watchdog is controlled using the watchdogTimeout(), watchdogEvents() and watchdogAbort().
    functions.

    The watchdog is implemented using a free running interval timer. The watchdog signal (\c SIGURG)
    must \e not be blocked. If signals need to be blocked for some reason, those regions will not be
    checked by the watchdog. If a callback blocks, the watchdog has no chance to interrupt the
    process.

    \warning Since the watchdog is free running for performance reasons, every callback must expect
        signals to happen. Signals \e will certainly happen since the watchdog signal is generated
        periodically (which does not necessarily generate a watchdog event ...)

    Additional signals (\c SIGALRM) may occur when using using hires timers on kernel/glibc
    combinations which do not support timerfd(). On such systems, hires timers are implemented using
    POSIX timers which generate a considerable number of additional signals.

    \todo Fix the file support to use threads (?) fork (?) and a pipe so it works reliably even
        over e.g. NFS.
  */
namespace scheduler {

    /** \brief Event handler main loop

        This member must be called at some time to enter the event handler main loop. Only while
        this function is running any events are handled. The call will return if
        \li a callback calls terminate()
        \li the run queue becomes empty.
     */
    void process();

    /** \brief \c true, if scheduler is running, \c false otherwise */
    bool running();

    /** \brief Called by callbacks to terminate the main loop

        This member may be called by any callback to tell the main loop to terminate. The main loop
        will return to it's caller after the currently running callback returns.
     */
    void terminate();

    /** \brief Immediately rescheduler

        Calling yield() will cause the scheduler to terminate the current queue run and immediately
        rescheduler all pending tasks.
     */
    void yield();

    /** \brief Return timestamp of last event

        This is the timestamp, the last event has been signaled. This is the real time at which the
        event is delivered \e not the time it should have been delivered (in the case of timers).
     */
    ClockService::clock_type eventTime();

    /** \brief Return (approximate) current time

        This call will return the current time as far as it is already known to the scheduler. If
        the scheduler is running, this will return eventTime(), otherwise it will return
        ClockService::now(). While the scheduler is running, this will reduce the number of system
        calls.
     */
    ClockService::clock_type now();

    /** \brief Set watchdog timeout to \a ms milliseconds.

        Setting the watchdog timeout to 0 will disable the watchdog.
     */
    void watchdogTimeout(unsigned ms);

    /** \brief Current watchdog timeout in milliseconds */
    unsigned watchdogTimeout();

    /** \brief Number of watchdog events

        calling watchtogEvents() will reset the counter to 0
     */
    unsigned watchdogEvents();

    /** \brief Enable/disable abort on watchdog event.

        Calling watchdogAbort(\c true) will enable aborting the program execution on a watchdog
        event.
     */
    void watchdogAbort(bool flag);

    /** \brief Get current watchdog abort on event status */
    bool watchdogAbort();

    /** \brief Switch to using hi resolution timers

        By default, timers are implemented directly using epoll. This however restricts the timer
        resolution to that of the kernel HZ value.

        High resolution timers are implemented either using POSIX timers or, when available, using
        the Linux special \c timerfd() syscall.

        POSIX timers are delivered using signals. A high timer load this increases the signal load
        considerably. \c timerfd()'s are delivered on a file descriptor and thus don't have such a
        scalability issue.

        \warning The timer source must not be switched from a scheduler callback
     */
    void hiresTimers();

    /** \brief Switch back to using epoll for timing
        \see hiresTimers()
     */
    void loresTimers();

    /** \brief return \c true, if \c timerfd() timing is available, \c false otherwise
        \see hiresTimers()
     */
    bool haveScalableHiresTimers();

    /** \brief Return \c true, if using hires times, \c false otherwise
        \see hiresTimers() */
    bool usingHiresTimers();

    /** \brief Restart scheduler

        This call will restart all scheduler dispatchers (timers, signals, file descriptors). This
        is necessary after a fork().
        \warning This call will \e remove all registered events from the scheduler
     */
    void restart();

    /** \brief Return \c true, if no event is registered, \c false otherwise. */
    bool empty();

    /** \brief %scheduler specific time source for Utils/Logger framework

        This time source may be used to provide timing information for %log messages within the
        Utils/Logger framework. This time source will use Scheduler::eventTime() to provide timing
        information.

        \code
        senf::log::timeSource<senf::scheduler::LogTimeSource>();
        \endcode

        Using this information reduces the number of necessary ClockService::now() calls and thus
        the number of system calls.
     */
    struct LogTimeSource : public senf::log::TimeSource
    {
        senf::log::time_type operator()() const;
    };

    /** \brief Temporarily block all signals

        This class is used to temporarily block all signals in a critical section.

        \code
        // Begin critical section
        {
            senf::scheduler::BlockSignals signalBlocker;

            // critical code executed with all signals blocked
        }
        // End critical section
        \endcode

        You need to take care not to block since even the watchdog timer will be disabled while
        executing within a critical section.
     */
    class BlockSignals
        : boost::noncopyable
    {
    public:
        BlockSignals(bool initiallyBlocked=true);
                                        ///< Block signals until end of scope
                                        /**< \param[in] initiallyBlocked set to \c false to not
                                             automatically block signals initially */
        ~BlockSignals();                ///< Release all signal blocks

        void block();                   ///< Block signals if not blocked
        void unblock();                 ///< Unblock signals if blocked
        bool blocked() const;           ///< \c true, if signals currently blocked, \c false
                                        ///< otherwise

    private:
        bool blocked_;
        sigset_t allSigs_;
        sigset_t savedSigs_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Scheduler.cci"
//#include "Scheduler.ct"
//#include "Scheduler.cti"
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
