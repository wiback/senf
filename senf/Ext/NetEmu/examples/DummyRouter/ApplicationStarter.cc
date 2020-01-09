// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
