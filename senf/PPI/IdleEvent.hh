//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
