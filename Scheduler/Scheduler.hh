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

/** \brief The Scheduler interface

    The %scheduler API is comprised of two parts:

    \li Specific event classes, one for each type of event.
    \li Some generic functions implemented in the \ref senf::scheduler namespace.

    Events are registered via the respective event class. The (global) functions are used to enter
    the application main-loop or query for global information.


    \section sched_handlers Specifying handlers

    All handlers are specified as generic <a
    href="http://www.boost.org/doc/html/function.html">Boost.Function</a> objects. This allows to
    pass any callable as a handler. Depending on the type of handler, some additional arguments may
    be passed to the handler by the %scheduler.

    If you need to pass additional information to your handler, use <a
    href="http://www.boost.org/libs/bind/bind.html">Boost.Bind</a>:
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
    href="http://www.boost.org/libs/bind/bind.html">Boost.Bind</a> or senf::membind()
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


    \section sched_fd Registering events

    Events are registered by allocating an instance of the corresponding event class:

    \li senf::scheduler::FdEvent for file descriptor events
    \li senf::scheduler::TimerEvent for single-shot deadline timer events
    \li senf::scheduler::SignalEvent for UNIX signal events

    The destructor of each of these classes will ensure, that the event will be properly
    unregistered. The registration can be enabled, disabled or changed using appropriate
    members. See the event class for details on a specific type of event.
    
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
