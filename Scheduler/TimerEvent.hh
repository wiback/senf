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
    \brief TimerDispatcher public header */

#ifndef HH_TimerDispatcher_
#define HH_TimerDispatcher_ 1

// Custom includes
#include <signal.h>
#include "../boost/intrusive/iset_hook.hpp"
#include "ClockService.hh"
#include "FdManager.hh"
#include "FIFORunner.hh"
#include "../Utils/Logger/SenfLog.hh"

//#include "TimerEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    namespace detail {
        struct TimerSetTag;
        typedef boost::intrusive::iset_base_hook<TimerSetTag> TimerSetBase;
        struct TimerSetCompare;
        class TimerDispatcher;
    }

    class TimerEvent
        : public FIFORunner::TaskInfo,
          public detail::TimerSetBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        TimerEvent(std::string const & name, Callback const & cb, ClockService::clock_type timeout,
                   bool initiallyEnabled = true);
        TimerEvent(std::string const & name, Callback const & cb);
        ~TimerEvent();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void enable();
        void disable();
        bool enabled();

        void action(Callback const & cb);
        void timeout(ClockService::clock_type timeout, bool enable=true);

    private:
        virtual void run();

        Callback cb_;
        ClockService::clock_type timeout_;

        friend class detail::TimerDispatcher;
        friend class detail::TimerSetCompare;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "TimerEvent.cci"
//#include "TimerEvent.ct"
//#include "TimerEvent.cti"
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
