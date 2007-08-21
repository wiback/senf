// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief ClockService non-inline non-template implementation */

#include "ClockService.hh"
//#include "ClockService.ih"

// Custom includes
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include "Utils/Exception.hh"

//#include "ClockService.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

#define CheckErrno(op,args) if (op args < 0) throw SystemException(# op, errno)

struct senf::ClockService::Impl 
{
    Impl();

    void block();
    void unblock();

    struct Blocker {
        Blocker(Impl * i) : impl(i) { impl->block(); }
        ~Blocker() { impl->unblock(); }
        Impl * impl;
    };

    static void timer(int);

    struct sigaction oldaction;
    struct itimerval olditimer;
    sigset_t alrm_set;
};

prefix_ senf::ClockService::Impl::Impl()
{
    CheckErrno( sigemptyset, (&alrm_set) );
    CheckErrno( sigaddset, (&alrm_set, SIGALRM) );
}

prefix_ void senf::ClockService::Impl::block()
{
    CheckErrno( sigprocmask, (SIG_BLOCK, &alrm_set, 0) );
}

prefix_ void senf::ClockService::Impl::unblock()
{
    CheckErrno( sigprocmask, (SIG_UNBLOCK, &alrm_set, 0) );
}

prefix_ void senf::ClockService::Impl::timer(int)
{
    boost::posix_time::ptime time (boost::posix_time::microsec_clock::universal_time());
    if (ClockService::instance().checkSkew(time))
        ClockService::instance().clockSkew(
            time, ClockService::instance().heartbeat_ + boost::posix_time::seconds(
                ClockService::CheckInterval));
    ClockService::instance().heartbeat_ = time;
}

///////////////////////////////////////////////////////////////////////////
// senf::ClockService

prefix_ senf::ClockService::~ClockService()
{
    setitimer(ITIMER_REAL, &impl_->olditimer, 0);
    sigaction(SIGALRM, &impl_->oldaction, 0);
}

////////////////////////////////////////
// private members

prefix_ senf::ClockService::ClockService()
    : base_ (boost::posix_time::microsec_clock::universal_time()), 
      heartbeat_ (base_), impl_(new ClockService::Impl())
{
    struct sigaction action;
    action.sa_handler = & senf::ClockService::Impl::timer;
    CheckErrno( sigemptyset, (&action.sa_mask) );
    action.sa_flags = SA_RESTART;
    CheckErrno( sigaction, (SIGALRM, &action, &impl_->oldaction) );

    struct itimerval itimer;
    itimer.it_interval.tv_sec = CheckInterval;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = CheckInterval;
    itimer.it_value.tv_usec = 0;
    CheckErrno( setitimer, (ITIMER_REAL, &itimer, &impl_->olditimer) );
    impl_->unblock();
}

prefix_ void senf::ClockService::restart_i()
{
    impl_->block(); // if any syscall fails, the alarm signal stays blocked which is correct
    base_ = boost::posix_time::microsec_clock::universal_time();
    heartbeat_ = base_;

    struct sigaction action;
    action.sa_handler = & senf::ClockService::Impl::timer;
    CheckErrno( sigemptyset, (&action.sa_mask) );
    action.sa_flags = SA_RESTART;
    CheckErrno( sigaction, (SIGALRM, &action, 0) );

    struct itimerval itimer;
    itimer.it_interval.tv_sec = CheckInterval;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = CheckInterval;
    itimer.it_value.tv_usec = 0;
    CheckErrno( setitimer, (ITIMER_REAL, &itimer, 0) );
    impl_->unblock();
}

prefix_ void senf::ClockService::updateSkew(boost::posix_time::ptime time)
{
    Impl::Blocker alrmBlocker (impl_.get());
    struct itimerval itimer;
    CheckErrno( getitimer, (ITIMER_REAL, &itimer) );
    clockSkew(time, (heartbeat_ 
                     + boost::posix_time::seconds(CheckInterval) 
                     - boost::posix_time::seconds(itimer.it_value.tv_sec)
                     - boost::posix_time::microseconds(itimer.it_value.tv_usec)));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ClockService.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
