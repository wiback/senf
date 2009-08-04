// $Id$
//
// Copyright (C) 2009 
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
    \brief IdleEvent public header */

#ifndef HH_SENF_Scheduler_IdleEvent_
#define HH_SENF_Scheduler_IdleEvent_ 1

// Custom includes
#include <boost/function.hpp>
#include <boost/intrusive/ilist_hook.hpp>
#include "FIFORunner.hh"

//#include "IdleEvent.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

namespace detail {
    struct IdleEventListTag;
    typedef boost::intrusive::ilist_base_hook<IdleEventListTag> IdleEventListBase;
    class IdleEventDispatcher;
}

    class IdleEvent
        : public detail::FIFORunner::TaskInfo,
          public detail::IdleEventListBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///\{
        
        IdleEvent(std::string const & name, Callback const & cb, 
                  bool initiallyEnabled = true);
        ~IdleEvent();

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        void disable();                 ///< Disable event
        void enable();                  ///< Enable event

        void action(Callback const & cb); ///< Change event callback

    protected:

    private:
        virtual void v_run();
        virtual char const * v_type() const;
        virtual std::string v_info() const;

        Callback cb_;
        
        friend class detail::IdleEventDispatcher;
    };
        
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "IdleEvent.cci"
//#include "IdleEvent.ct"
//#include "IdleEvent.cti"
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
