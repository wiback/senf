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
    \brief Poller public header */

#ifndef HH_Poller_
#define HH_Poller_ 1

// Custom includes
#include <sys/epoll.h>
#include <boost/utility.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>

//#include "Poller.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

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
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef Value value_type;
        typedef boost::transform_iterator<GetPollResult, epoll_event*> iterator;
        typedef boost::iterator_range<iterator> range;

        enum Events { 
            EV_READ = EPOLLIN, EV_PRIO = EPOLLPRI, EV_WRITE = EPOLLOUT,
            EV_HUP = EPOLLHUP, EV_ERR = EPOLLERR 
};

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        Poller();
        ~Poller();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

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


}}

///////////////////////////////hh.e////////////////////////////////////////
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
