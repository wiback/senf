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
    \brief ApplicationStarter non-inline non-template implementation */

#include "ApplicationStarter.hh"
//#include "ApplicationStarter.ih"

// Custom includes
#include <senf/Utils/Console.hh>
#include <senf/Ext/NetEmu/EmulatedWirelessInterface.hh>

//#include "ApplicationStarter.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

wiback::ApplicationStarter::ApplicationStarter(senf::ClockService::clock_type delay, Callback cb)
    : timer_ ("application startup", senf::membind(&wiback::ApplicationStarter::delay, this)),
      delay_ (delay), cb_ (cb), linked_ (true)
{
    namespace fty = senf::console::factory;

    senf::console::sysdir()["emu"]["ipc"]("start").unlink();
    senf::console::sysdir()["emu"]["ipc"].add(
        "start", fty::Command(&wiback::ApplicationStarter::start, this));
    SENF_LOG(("Waiting for 'start'"));
}

prefix_ wiback::ApplicationStarter::~ApplicationStarter()
{
    reset();
}

prefix_ void wiback::ApplicationStarter::start()
{
    senf::emu::EmulatedFrequencyRegistry::instance().start();
    reset();
    timer_.timeout(senf::ClockService::now()+delay_);
}

prefix_ void wiback::ApplicationStarter::delay()
{
    SENF_LOG(("Starting application"));
    cb_();
}

prefix_ void wiback::ApplicationStarter::reset()
{
    namespace fty = senf::console::factory;

    if (! linked_)
        return;
    senf::console::sysdir()["emu"]["ipc"]("start").unlink();
    senf::console::sysdir()["emu"]["ipc"].add(
        "start", fty::Command(&senf::emu::EmulatedFrequencyRegistry::start,
                              &senf::emu::EmulatedFrequencyRegistry::instance()));
    linked_ = false;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "ApplicationStarter.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
