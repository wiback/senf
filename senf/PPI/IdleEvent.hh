// $Id$
//
// Copyright (C) 2007
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
    \brief IdleEvent public header */

#ifndef HH_SENF_PPI_IdleEvent_
#define HH_SENF_PPI_IdleEvent_ 1

// Custom includes
#include <senf/Scheduler/TimerEvent.hh>
#include "Events.hh"

//#include "IdleEvent.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Continually signaled event

        An IdleEvent is signaled continually and repeatedly while enabled. It will consume 100% of
        available CPU resources. The resource usage is controlled by adequate event throttling.

        \ingroup event_group
     */
    class IdleEvent
        : public EventImplementation<>
    {
    public:
        IdleEvent(std::string const & name = "");

    private:
        virtual void v_enable();
        virtual void v_disable();

        void cb();

        scheduler::TimerEvent timer_;
    };

}}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
