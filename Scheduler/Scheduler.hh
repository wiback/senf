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
#include <map>
#include <queue>
#include <boost/function.hpp>
#include <boost/utility.hpp>
#include <boost/call_traits.hpp>
#include <boost/integer.hpp>
#include "ClockService.hh"

//#include "scheduler.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief SENF Project namespace */
namespace senf {

    /** \brief Singleton class to manage the event loop

        This class manages a single select() type event loop. A customer of this class may register
        any number of file descriptors with this class and pass callback functions to be called on
        input, output or error. This functions are specified using boost::function objects (See <a
        href="http://www.boost.org/doc/html/function.html">Boost.Function</a>)

        The Scheduler is based on a generic handle representation. The only information needed from
        a handle, is the intrinsic file descriptor. Any object for which the statement
        \code
          int fd = retrieve_filehandle(object);
        \endcode
        is valid and places the relevant file descriptor into fd can be used as a Handle type. There
        is an implementation of retrieve_filehandle(int) within the library to handle explicit file
        descriptors. The <a href="../../../Socket/doc/html/index.html">Socket library</a> provides an
        implementation of <tt>retrieve_filehandle(FileHandle handle)</tt>. If you want to support
        some other handle type, just define an appropriate \c retrieve_filehandle function <em>in
        that types namespace</em>.

        It is important to note, that for every combination of file descriptor and event, only a \e
        single handler may be installed. Installing more handlers does not make sense. If you need
        to distribute data to several interested parties, you must take care of this yourself.

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
         */
        template <class Handle>
        struct GenericCallback {
            typedef boost::function<void (typename boost::call_traits<Handle>::param_type,
                                          EventId) > Callback;
        };

        /** \brief Callback type for timer events */
        typedef boost::function<void ()> TimerCallback;

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

        template <class Handle>
        void add(Handle const & handle,
                 typename GenericCallback<Handle>::Callback const & cb,
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

        unsigned timeout(ClockService::clock_type timeout, TimerCallback const & cb); 
                                        ///< Add timeout event
                                        /**< \param[in] timeout timeout in nanoseconds
                                             \param[in] cb callback to call after \a timeout
                                                 milliseconds
                                             \todo Return some kind of handle/pointer and add
                                                 support to update or revoke a timeout */

        void cancelTimeout(unsigned id);

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
        typedef boost::function<void (EventId)> SimpleCallback;

        Scheduler();

        void do_add(int fd, SimpleCallback const & cb, int eventMask = EV_ALL);
        void do_remove(int fd, int eventMask = EV_ALL);

        /** \brief Descriptor event specification
            \internal */
        struct EventSpec
        {
            SimpleCallback cb_read;
            SimpleCallback cb_prio;
            SimpleCallback cb_write;

            int epollMask() const;
        };

        /** \brief Timer event specification
            \internal */
        struct TimerSpec
        {
            TimerSpec() : timeout(), cb() {}
            TimerSpec(ClockService::clock_type timeout_, TimerCallback cb_, unsigned id_)
                : timeout(timeout_), cb(cb_), id(id_), canceled(false) {}

            bool operator< (TimerSpec const & other) const
                { return timeout > other.timeout; }

            ClockService::clock_type timeout;
            TimerCallback cb;
            unsigned id;
            bool canceled;
        };

        typedef std::map<int,EventSpec> FdTable;
        typedef std::map<unsigned,TimerSpec> TimerMap; // sorted by id

        struct TimerSpecCompare
        {
            typedef TimerMap::iterator first_argument_type;
            typedef TimerMap::iterator second_argument_type;
            typedef bool result_type;
            
            result_type operator()(first_argument_type a, second_argument_type b);
        };

        typedef std::priority_queue<TimerMap::iterator, std::vector<TimerMap::iterator>, 
                                    TimerSpecCompare> TimerQueue; // sorted by time

        FdTable fdTable_;
        unsigned timerIdCounter_;
        TimerQueue timerQueue_;
        TimerMap timerMap_;
        int epollFd_;
        bool terminate_;
        ClockService::clock_type eventTime_;
    };

    /** \brief Default file descriptor accessor

        retrieve_filehandle() provides the Scheduler with support for explicit file descriptors as
        file handle argument.

        \relates Scheduler
     */
    int retrieve_filehandle(int fd);

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
