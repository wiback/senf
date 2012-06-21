// $Id$
//
// Copyright (C) 2008
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
    \brief Poller public header */

#ifndef HH_SENF_Scheduler_Poller_
#define HH_SENF_Scheduler_Poller_ 1

// Custom includes
#include <sys/epoll.h>
#include <boost/noncopyable.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>

//#include "Poller.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {
namespace detail {

    /** \brief Epoll abstraction

        This class provides a more convenient interface to the epoll() API. File descriptors are
        registered with pointers to a parameterized event type. After waiting for an event, the
        Poller allows to iterate over the event instances for all posted events.

        \tparam Value Event type
      */
    template <class Value>
    class Poller
        : boost::noncopyable
    {
        struct GetPollResult
        {
            typedef std::pair<int, Value*> result_type;
            result_type operator()(epoll_event const &) const;
        };

        static int const NumEvents = 8;

    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef Value value_type;
        typedef boost::transform_iterator<GetPollResult, epoll_event*> iterator;
        typedef boost::iterator_range<iterator> range;

        enum Events {
            EV_READ = EPOLLIN, EV_PRIO = EPOLLPRI, EV_WRITE = EPOLLOUT,
            EV_HUP = EPOLLHUP, EV_ERR = EPOLLERR
        };

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        Poller();
        ~Poller();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        bool set(int fd, int events, Value * data); ///< Set file descriptor event data and mask
                                        /**< The Poller does \e not own \a data. The value is owned
                                             by some external entity (the dispatcher to be more
                                             precise). */
        void remove(int fd);            ///< Remove file descriptor
        range wait();                   ///< Wait for one event
                                        /**< \returns a range of iterators which iterate over the
                                             data values registered with the event */

        void timeout(int t);            ///< Set event timeout to \a t milliseconds
        int timeout() const;            ///< Current event timeout

    private:
        int epollFd_;
        int timeout_;
    };


}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Poller.cci"
#include "Poller.ct"
#include "Poller.cti"
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
