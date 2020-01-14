//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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

        bool set(int fd, int events, Value * data);
                                        ///< Set file descriptor event data and mask
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
