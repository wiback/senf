// $Id$
//
// Copyright (C) 2006
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
    \brief Scheduler public header
 */

#ifndef HH_Scheduler_
#define HH_Scheduler_ 1

// Custom includes
#include "../Utils/Logger/SenfLog.hh"
#include "FdEvent.hh"
#include "TimerEvent.hh"
#include "SignalEvent.hh"

//#include "scheduler.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

/** \brief Visible scheduler interface

    The %scheduler singleton manages access to the %scheduler library. It provides access to
    several event dispatchers:
    \li File descriptor notifications
    \li Timeouts
    \li UNIX Signals

    The %scheduler is entered by calling it's process() member. This call will continue to run as
    long as there is something to do, or until one of the handlers calls terminate(). The
    %scheduler has 'something to do' as long as there is any file descriptor or timeout active.

    The %scheduler only provides low level primitive scheduling capability. Additional helpers
    are defined on top of this functionality (e.g. ReadHelper or WriteHelper or the interval
    timers of the PPI).


    \section sched_handlers Specifying handlers

    All handlers are passed as generic <a
    href="http://www.boost.org/doc/html/function.html">Boost.Function</a> objects. This allows
    to pass any callable as a handler. Depending on the type of handler, some additional
    arguments may be passed to the handler by the %scheduler. 

    If you need to pass additional information to your handler, use <a
    href="http://www.boost.org/libs/bind/bind.html">Boost.Bind</a>:
    \code
    // Handle callback function
    void callback(UDPv4ClientSocketHandle handle, senf::Scheduler::EventId event) {..}
    // Pass 'handle' as additional first argument to callback()
    Scheduler::instance().add(handle, boost::bind(&callback, handle, _1), EV_READ)
     // Timeout function
    void timeout( int n) {..}
    // Call timeout() handler with argument 'n'
    Scheduler::instance().timeout(boost::bind(&timeout, n))
    \endcode

    To use member-functions as callbacks, use either <a
    href="http://www.boost.org/libs/bind/bind.html">Boost.Bind</a> or senf::membind()
    \code
    // e.g. in Foo::Foo() constructor:
    Scheduler::instance().add(handle_, senf::membind(&Foo::callback, this)), EV_READ)
    \endcode

    The handler can also be identified by an arbitrary, user specified name. This name is used
    in error messages to identify the failing handler.


    \section sched_fd Registering file descriptors
    
    File descriptors are managed using add() or remove()
    \code
    Scheduler::instance().add(handle, &callback, EV_ALL);
    Scheduler::instance().remove(handle);
    \endcode 

    The callback will be called with one additional argument. This argument is the event mask of
    type EventId. This mask will tell, which of the registered events are signaled. The
    additional flags EV_HUP or EV_ERR (on hangup or error condition) may be set additionally.

    Only a single handler may be registered for any combination of file descriptor and event
    (registering multiple callbacks for a single fd and event does not make sense).

    The %scheduler will accept any object as \a handle argument as long as retrieve_filehandle()
    may be called on that object
    \code
    int fd = retrieve_filehandle(handle);
    \endcode 
    to fetch the file handle given some abstract handle type. retrieve_filehandle() will be
    found using ADL depending on the argument namespace. A default implementation is provided
    for \c int arguments (file descriptors)


    \section sched_timers Registering timers

    The %scheduler has very simple timer support. There is only one type of timer: A single-shot
    deadline timer. More complex timers are built based on this. Timers are managed using
    timeout() and cancelTimeout()
    \code
    int id = Scheduler::instance().timeout(Scheduler::instance().eventTime() + ClockService::milliseconds(100),
                                           &callback);
    Scheduler::instance().cancelTimeout(id);
    \endcode 
    Timing is based on the ClockService, which provides a high resolution and strictly
    monotonous time source which again is based on POSIX timers. Registering a timeout will fire
    the callback when the target time is reached. The timer may be canceled by passing the
    returned \a id to cancelTimeout().


    \section sched_signals Registering POSIX/UNIX signals

    The %scheduler also incorporates standard POSIX/UNIX signals. Signals registered with the
    %scheduler will be handled \e synchronously within the event loop.
    \code
    Scheduler::instance().registerSignal(SIGUSR1, &callback);
    Scheduler::instance().unregisterSignal(SIGUSR1);
    \endcode
    When registering a signal with the %scheduler, that signal will automatically be blocked so
    it can be handled within the %scheduler. 

    A registered signal does \e not count as 'something to do'. It is therefore not possible to
    wait for signals \e only.

    \todo Change the Scheduler API to use RAII. Additionally, this will remove all dynamic
        memory allocations from the scheduler.
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

    /** \brief Called by callbacks to terminate the main loop

        This member may be called by any callback to tell the main loop to terminate. The main loop
        will return to it's caller after the currently running callback returns. 
     */
    void terminate(); 

    /** \brief Return date/time of last event

        This is the timestamp, the last event has been signaled. This is the real time at which the
        event is delivered \e not the time it should have been delivered (in the case of timers). 
     */
    ClockService::clock_type eventTime(); 

    /** \brief Set task watchdog timeout */
    void taskTimeout(unsigned ms); 

    /** \brief Current task watchdog timeout */
    unsigned taskTimeout(); 

    /** \brief Number of watchdog events */
    unsigned hangCount(); 

    /** \brief Restart scheduler
        
        This call will restart all scheduler dispatchers (timers, signals, file descriptors). This
        is necessary after a fork().
        \warning This call will \e remove all registered events from the scheduler
     */
    void restart(); 

    /** \brief %scheduler specific time source for Utils/Logger framework

        This time source may be used to provide timing information for log messages within the
        Utils/Logger framework. This time source will use Scheduler::eventTime() to provide timing
        information.

        Using this information reduces the number of necessary ClockService::now() calls and thus
        the number of system calls.
     */
    struct LogTimeSource : public senf::log::TimeSource
    {
        senf::log::time_type operator()() const;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
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
