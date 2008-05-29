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
    \brief Timer public header */

#ifndef HH_Timer_
#define HH_Timer_ 1

// Custom includes
#include "Scheduler.hh"

//#include "Timer.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Manage scheduler timer

        This class will manage a single timer: The timer can be enabled, disabled and updated and
        will automatically be removed, when this instance is destroyed.

        \code
        class Foo
        {
        public:
            Foo() : timer_ ( ClockServer::now() + ClockService::milliseconds(500),
                             senf::membind(&Foo::timer, this) ) {}

            void blarf() { timer_.disable(); }
        
        private:
            void timer(); 
        
            senf::SchedulerTimer timer_;
        };
        \endcode
      */
    class SchedulerTimer
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        SchedulerTimer(ClockService::clock_type timeout, Scheduler::SimpleCallback const & cb,
                       bool enabled=true);
        ~SchedulerTimer();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void enable();                  ///< Enable timer
        void disable();                 ///< Disable timer
        bool enabled();                 ///< \c true, if timer is currently enabled
                                        /**< An expired timer can still be in enabled state. */

        void update(ClockService::clock_type timeout); ///< Change timeout time and enable timer
                                        /**< If the timer is not enabled, you need to call enable()
                                             for the timer to become effective. */

    protected:

    private:
        ClockService::clock_type timeout_;
        Scheduler::SimpleCallback cb_;
        unsigned id_;
        bool enabled_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "Timer.cci"
//#include "Timer.ct"
//#include "Timer.cti"
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
