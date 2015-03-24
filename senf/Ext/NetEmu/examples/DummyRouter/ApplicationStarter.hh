// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief ApplicationStarter public header */

#ifndef HH_SENF_Ext_NetEmu_examples_DummyRouter_ApplicationStarter_
#define HH_SENF_Ext_NetEmu_examples_DummyRouter_ApplicationStarter_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Utils/Logger.hh>

//#include "ApplicationStarter.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace wiback {

    class ApplicationStarter : boost::noncopyable
    {
        SENF_LOG_CLASS_AREA();
    public:
        typedef boost::function<void ()> Callback;

        ApplicationStarter(senf::ClockService::clock_type delay, Callback cb);
        ~ApplicationStarter();

    private:
        void start();
        void delay();
        void reset();

        senf::scheduler::TimerEvent timer_;
        senf::ClockService::clock_type delay_;
        Callback cb_;
        bool linked_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ApplicationStarter.cci"
//#include "ApplicationStarter.ct"
//#include "ApplicationStarter.cti"
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
