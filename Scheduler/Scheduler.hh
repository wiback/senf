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
#include "FdDispatcher.hh"
#include "TimerDispatcher.hh"
#include "SignalEvent.hh"
#include "FileDispatcher.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "scheduler.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief SENF Project namespace */
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
    class Scheduler
        : boost::noncopyable
    {
    public:

        SENF_LOG_CLASS_AREA();

        ///////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief Types of file descriptor events 

            These events are grouped into to classes:
            \li Ordinary file descriptor events for which handlers may be registered. These are
                EV_READ, EV_PRIO and EV_WRITE. EV_ALL is a combination of these three.
            \li Error flags. These additional flags may be passed to a handler to pass an error
                condition to the handler. 
         */
        enum EventId {
            EV_NONE  = 0                              /**< No event */
          , EV_READ  = scheduler::FdManager::EV_READ  /**< File descriptor is readable */
          , EV_PRIO  = scheduler::FdManager::EV_PRIO  /**< File descriptor has OOB data */
          , EV_WRITE = scheduler::FdManager::EV_WRITE /**< File descriptor is writable */
          , EV_ALL   = scheduler::FdManager::EV_READ
                     | scheduler::FdManager::EV_PRIO
                     | scheduler::FdManager::EV_WRITE /**< Used to register all events at once
                                                           (read/prio/write) */
          , EV_HUP   = scheduler::FdManager::EV_HUP   /**< Hangup condition on file handle */
          , EV_ERR   = scheduler::FdManager::EV_ERR   /**< Error condition on file handle */
        };

        /** \brief Callback type for file descriptor events */
        typedef boost::function<void (int)> FdCallback;

        /** \brief Callback type for timer events */
        typedef boost::function<void ()> SimpleCallback;

        /** \brief Callback type for signal events */
        typedef boost::function<void (siginfo_t const &)> SignalCallback;

        /** \brief Timer id type */
        typedef scheduler::TimerDispatcher::timer_id timer_id;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // private default constructor
        // no copy constructor
        // no copy assignment
        // default destructor
        // no conversion constructors

        /** \brief Return %scheduler instance

            This static member is used to access the singleton instance. This member is save to
            return a correctly initialized %scheduler instance even if called at global construction
            time
         */
        static Scheduler & instance();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name File Descriptors
        ///\{

        template <class Handle>
        void add(std::string const & name, Handle const & handle, FdCallback const & cb,
                 int eventMask = EV_ALL);  ///< Add file handle event callback
                                        /**< add() will add a callback to the %scheduler. The
                                             callback will be called for the given type of event on
                                             the given  arbitrary file-descriptor or
                                             handle-like object. If there already is a Callback
                                             registered for one of the events requested, the new
                                             handler will replace the old one.
                                             \param[in] name descriptive name to identify the
                                                 callback.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] cb callback
                                             \param[in] eventMask arbitrary combination via '|'
                                                 operator of \ref senf::Scheduler::EventId "EventId"
                                                 designators. */
 
        template <class Handle>        
        void add(Handle const & handle, FdCallback const & cb,
                 int eventMask = EV_ALL); ///< Add file handle event callback
                                        /**< \see add() */


        template <class Handle>
        void remove(Handle const & handle, int eventMask = EV_ALL); ///< Remove event callback
                                        /**< remove() will remove any callback registered for any of
                                             the given events on the given file descriptor or handle
                                             like object.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] eventMask arbitrary combination via '|'
                                                 operator of \ref senf::Scheduler::EventId "EventId"
                                                 designators. */

        ///\}

        ///\name Timeouts
        ///\{

        timer_id timeout(std::string const & name, ClockService::clock_type timeout, 
                         SimpleCallback const & cb); 
                                        ///< Add timeout event
                                        /**< \returns timer id
                                             \param[in] name descriptive name to identify the
                                                 callback.
                                             \param[in] timeout timeout in nanoseconds
                                             \param[in] cb callback to call after \a timeout
                                                 milliseconds */

        timer_id timeout(ClockService::clock_type timeout, SimpleCallback const & cb); 
                                        ///< Add timeout event
                                        /**< \see timeout() */

        void cancelTimeout(timer_id id); ///< Cancel timeout \a id

#ifndef DOXYGEN
        ClockService::clock_type timeoutEarly() const;
        void timeoutEarly(ClockService::clock_type v);

        ClockService::clock_type timeoutAdjust() const;
        void timeoutAdjust(ClockService::clock_type v);
#endif

        ///\}

        void process();                 ///< Event handler main loop
                                        /**< This member must be called at some time to enter the
                                             event handler main loop. Only while this function is
                                             running any events are handled. The call will return
                                             only, if any callback calls terminate(). */

        void terminate();               ///< Called by callbacks to terminate the main loop
                                        /**< This member may be called by any callback to tell the
                                             main loop to terminate. The main loop will return to
                                             it's caller after the currently running callback
                                             returns. */
        
        ClockService::clock_type eventTime() const; ///< Return date/time of last event
                                        /**< This is the timestamp, the last event has been
                                             signaled. This is the real time at which the event is
                                             delivered \e not the time it should have been delivered
                                             (in the case of timers). */

        void taskTimeout(unsigned ms);
        unsigned taskTimeout() const;
        unsigned hangCount() const;

        void restart();

    protected:

    private:
        Scheduler();

        void do_add(int fd, FdCallback const & cb, int eventMask = EV_ALL);
        void do_add(std::string const & name, int fd, FdCallback const & cb, 
                    int eventMask = EV_ALL);
        void do_remove(int fd, int eventMask);

        bool terminate_;

        scheduler::FdDispatcher fdDispatcher_;
        scheduler::TimerDispatcher timerDispatcher_;
        scheduler::FileDispatcher fileDispatcher_;
    };

    /** \brief Default file descriptor accessor

        retrieve_filehandle() provides the %scheduler with support for explicit file descriptors as
        file handle argument.

        \relates Scheduler
     */
    int retrieve_filehandle(int fd);

    /** \brief %scheduler specific time source for Utils/Logger framework

        This time source may be used to provide timing information for log messages within the
        Utils/Logger framework. This time source will use Scheduler::eventTime() to provide timing
        information.
     */
    struct SchedulerLogTimeSource : public senf::log::TimeSource
    {
        senf::log::time_type operator()() const;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "Scheduler.cci"
//#include "Scheduler.ct"
#include "Scheduler.cti"
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
