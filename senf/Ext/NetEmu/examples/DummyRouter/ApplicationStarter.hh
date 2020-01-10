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
