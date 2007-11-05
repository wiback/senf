// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
#include <signal.h>
#include <setjmp.h>
#include <map>
#include <queue>
#include <boost/function.hpp>
#include <boost/utility.hpp>
#include <boost/call_traits.hpp>
#include <boost/integer.hpp>
#include "ClockService.hh"
#include "../Utils/Logger/Target.hh"

//#include "scheduler.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief SENF Project namespace */
namespace senf {

    /** \brief Singleton class to manage the event loop

        The Scheduler singleton manages the central event loop. It manages and dispatches all types
        of events managed by the scheduler library:
        \li File descriptor notifications
        \li Timeouts
        \li UNIX Signals

        The scheduler is entered by calling it's process() member. This call will continue to run as
        long as there is something to do, or until one of the handlers calls terminate(). The
        Scheduler has 'something to do' as long as there is any file descriptor or timeout active.

        The Scheduler only provides low level primitive scheduling capability. Additional helpers
        are defined on top of this functionality (e.g. ReadHelper or WriteHelper or the interval
        timers of the PPI).

        \section sched_handlers Handlers

        All handlers are managed as generic <a
        href="http://www.boost.org/doc/html/function.html">Boost.Function</a> objects. This allows
        to pass any callable as a handler. Depending on the type of handler, some additional
        arguments may be passed to the handler by the scheduler. If you want to pass additional
        arguments to the handler, use <a
        href="http://www.boost.org/libs/bind/bind.html">Boost.Bind</a>)).
        

        \section sched_fd File descriptors
        
        File descriptors are managed using add() or remove()
        \code
        Scheduler::instance().add(handle, &callback);
        Scheduler::instance().remove(handle);
        \endcode
        The callback will be called with one additional argument. This argument is the event mask of
        type EventId. This mask will tell, which of the registered events are
        signaled. Additionally, EV_HUP or EV_ERR on hangup or error condition on the handle.

        There is always only one handler registered for a file descriptor (registering multiple
        callbacks for a single fd does not make sense).

        The scheduler will accept an almost arbitrary object as it's first argument. It will use
        \code
        int fd = retrieve_filehandle(handle);
        \endcode
        To fetch the file handle given some abstract handle type. The definition of
        retrieve_filehandle() will be found using ADL.


        \section sched_timers Timers

        The Scheduler has very simple timer support. There is only one type of timer: A single-shot
        deadline timer. More complex timers are built based on this. Timers are managed using
        timeout() and cancelTimeout()
        \code
        int id = Scheduler::instance().timeout(Scheduler::instance().eventTime() + ClockService::milliseconds(100),
                                               &callback);
        Scheduler::instance().cancelTimeout(id);
        \endcode 
        Timing is based on the ClockService, which provides a high resolution and strictly
        monotonous time source. Registering a timeout will fire the callback when the target time is
        reached. The timer may be canceled by passing the returned \a id to cancelTimeout().

        There are two parameters which adjust the exact: \a timeoutEarly and \a timeoutAdjust. \a
        timeoutEarly is the time, a callback may be called before the deadline time is
        reached. Setting this value below the scheduling granularity of the kernel will have the
        scheduler go into a <em>busy wait</em> (that is, an endless loop consuming 100% of CPU
        recources) until the deadline time is reached! This is seldom desired. The default setting
        of 11ms is adequate in most cases (it's slightly above the lowest linux scheduling
        granularity). 

        The other timeout scheduling parameter is \a timeoutAdjust. This value will be added to the
        timeout value before calculating the next delay value thereby compensating for \a
        timeoutEarly. By default, this value is set to 0 but may be changed if needed.


        \section sched_signals POSIX/UNIX signals

        The Scheduler also incorporates standard POSIX/UNIX signals. Signals registered with the
        scheduler will be handled \e synchronously within the event loop.
        \code
        Scheduler::instance().registerSignal(SIGUSR1, &callback);
        Scheduler::instance().unregisterSignal(SIGUSR1);
        \endcode
        When registering a signal with the scheduler, that signal will automatically be blocked so
        it can be handled within the scheduler. 

        A registered signal does \e not count as 'something to do'. It is therefore not possible to
        wait for signals \e only.

        \todo Fix EventId parameter (probably to int) to allow |-ing without casting ...
      */
    class Scheduler
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief Types of file descriptor events */
        enum EventId { EV_NONE=0,
                       EV_READ=1, EV_PRIO=2, EV_WRITE=4, 
                       EV_ALL=7,
                       EV_HUP=8, EV_ERR=16 };

        /** \brief Template typedef for Callback type

            This is a template typedef (which does not exist in C++) that is, a template class whose
            sole member is a typedef symbol defining the callback type given the handle type.

            The Callback is any callable object taking a \c Handle and an \c EventId as argument.
        template <class Handle>
        struct GenericCallback {
            typedef boost::function<void (typename boost::call_traits<Handle>::param_type,
                                          EventId) > Callback;
        };
         */

        typedef boost::function<void (EventId)> FdCallback;

        /** \brief Callback type for timer events */
        typedef boost::function<void ()> SimpleCallback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // private default constructor
        // no copy constructor
        // no copy assignment
        // default destructor
        // no conversion constructors

        /** \brief Return Scheduler instance

            This static member is used to access the singleton instance. This member is save to
            return a correctly initialized Scheduler instance even if called at global construction
            time

            \implementation This static member just defines the Scheduler as a static method
                variable. The C++ standard then provides above guarantee. The instance will be
                initialized the first time, the code flow passes the variable declaration found in
                the instance() body.
         */
        static Scheduler & instance();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name File Descriptors
        ///\{

        template <class Handle>
        void add(Handle const & handle, FdCallback const & cb,
                 int eventMask = EV_ALL); ///< Add file handle event callback
                                        /**< add() will add a callback to the Scheduler. The
                                             callback will be called for the given type of event on
                                             the given  arbitrary file-descriptor or
                                             handle-like object. If there already is a Callback
                                             registered for one of the events requested, the new
                                             handler will replace the old one.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] cb callback
                                             \param[in] eventMask arbitrary combination via '|'
                                                 operator of EventId designators. */
        template <class Handle>
        void remove(Handle const & handle, int eventMask = EV_ALL); ///< Remove event callback
                                        /**< remove() will remove any callback registered for any of
                                             the given events on the given file descriptor or handle
                                             like object.
                                             \param[in] handle file descriptor or handle providing
                                                 the Handle interface defined above.
                                             \param[in] eventMask arbitrary combination via '|'
                                                 operator of EventId designators. */

        ///\}

        ///\name Timeouts
        ///\{

        unsigned timeout(ClockService::clock_type timeout, SimpleCallback const & cb); 
                                        ///< Add timeout event
                                        /**< \param[in] timeout timeout in nanoseconds
                                             \param[in] cb callback to call after \a timeout
                                                 milliseconds */

        void cancelTimeout(unsigned id); ///< Cancel timeout \a id

        ClockService::clock_type timeoutEarly() const;
                                        ///< Fetch the \a timeoutEarly parameter
        void timeoutEarly(ClockService::clock_type v);
                                        ///< Set the \a timeoutEarly parameter

        ClockService::clock_type timeoutAdjust() const;\
                                        ///< Fetch the \a timeoutAdjust parameter
        void timeoutAdjust(ClockService::clock_type v);
                                        ///< Set the \a timeoutAdjust parameter

        ///\}

        ///\name Signal handlers
        ///\{
        
        void registerSignal(unsigned signal, SimpleCallback const & cb);
                                        ///< Add signal handler
                                        /**< \param[in] signal signal number to register handler for
                                             \param[in] cb callback to call whenever \a signal is
                                                 delivered. */

        void unregisterSignal(unsigned signal);
                                        ///< Remove signal handler for \a signal

        struct InvalidSignalNumberException : public std::exception
        { virtual char const * what() const throw() 
                { return "senf::Scheduler::InvalidSignalNumberException"; } };


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

    protected:

    private:
        Scheduler();

        void do_add(int fd, FdCallback const & cb, int eventMask = EV_ALL);
        void do_remove(int fd, int eventMask = EV_ALL);

        void registerSigHandlers();
        static void sigHandler(int signal, ::siginfo_t * siginfo, void *);

#       ifndef DOXYGEN

        /** \brief Descriptor event specification
            \internal */
        struct EventSpec
        {
            FdCallback cb_read;
            FdCallback cb_prio;
            FdCallback cb_write;

            int epollMask() const;
        };

        /** \brief Timer event specification
            \internal */
        struct TimerSpec
        {
            TimerSpec() : timeout(), cb() {}
            TimerSpec(ClockService::clock_type timeout_, SimpleCallback cb_, unsigned id_)
                : timeout(timeout_), cb(cb_), id(id_), canceled(false) {}

            bool operator< (TimerSpec const & other) const
                { return timeout > other.timeout; }

            ClockService::clock_type timeout;
            SimpleCallback cb;
            unsigned id;
            bool canceled;
        };

#       endif 

        typedef std::map<int,EventSpec> FdTable;
        typedef std::map<unsigned,TimerSpec> TimerMap; // sorted by id

#       ifndef DOXYGEN

        struct TimerSpecCompare
        {
            typedef TimerMap::iterator first_argument_type;
            typedef TimerMap::iterator second_argument_type;
            typedef bool result_type;
            
            result_type operator()(first_argument_type a, second_argument_type b);
        };

#       endif

        typedef std::priority_queue<TimerMap::iterator, std::vector<TimerMap::iterator>, 
                                    TimerSpecCompare> TimerQueue; // sorted by time

        typedef std::vector<SimpleCallback> SigHandlers;

        FdTable fdTable_;

        unsigned timerIdCounter_;
        TimerQueue timerQueue_;
        TimerMap timerMap_;

        SigHandlers sigHandlers_;
        ::sigset_t sigset_;
        int sigpipe_[2];

        int epollFd_;
        bool terminate_;
        ClockService::clock_type eventTime_;
        ClockService::clock_type eventEarly_;
        ClockService::clock_type eventAdjust_;
    };

    /** \brief Default file descriptor accessor

        retrieve_filehandle() provides the Scheduler with support for explicit file descriptors as
        file handle argument.

        \relates Scheduler
     */
    int retrieve_filehandle(int fd);

    /** \brief Scheduler specific time source for Utils/Logger framework

        This time source may be used to provide timing information for log messages within the
        Utils/Logger framework. This time source will use Scheduler::eventTime() to provide timing
        information.
     */
    struct SchedulerLogTimeSource : public senf::log::TimeSource
    {
        boost::posix_time::ptime operator()() const;
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
