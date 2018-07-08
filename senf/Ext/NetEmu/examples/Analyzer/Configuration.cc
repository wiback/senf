// $Id:$
//
// Copyright (C) 2012
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
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>


// Definition of non-inline non-template functions

// Custom includes
extern "C" {
  #include <sched.h>
  #include <sys/mman.h>
  #include <sys/utsname.h>
}

// Custom includes
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Utils/Console.hh>
#include <senf/Scheduler/Scheduler.hh>
#include "Configuration.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ Configuration::Configuration() :
    verbose( false),
    numPackets(10),
    pktSize(1000),
    sendBuffer(256*1024),
    interface( "eth0"),
    destination(senf::MACAddress::None),
    sessionId(1)
{ 
    namespace fty = senf::console::factory;
    namespace kw = senf::console::kw;
    senf::console::DirectoryNode & initDir (senf::console::root().add("init", fty::Directory()));
    initDir.add("help", fty::Command( &Configuration::help, this));
    initDir.add("version", fty::Command( &Configuration::version, this));
    initDir.add("realtime-scheduling", fty::Command( &Configuration::enableRealtimeScheduling, this));
    initDir.add("verbose", fty::Variable( verbose));
    initDir.add("num-packets", fty::Variable( numPackets));
    initDir.add("packet-size", fty::Variable( pktSize));
    initDir.add("send-buffer", fty::Variable( sendBuffer));
    initDir.add("interface", fty::Variable( interface));
    initDir.add("destination", fty::Variable( destination));
    initDir.add("sessionId", fty::Variable( sessionId));
    
    // always turn those on, where available
    enableHighresTimers();
    enableRealtimeScheduling();
}

prefix_ void Configuration::help()
{
    senf::console::DirectoryNode::ChildrenRange range(senf::console::root()["init"].children());

    senf::console::DirectoryNode::ChildrenRange::iterator it;
    for (it = range.begin(); it != range.end(); it++) {
        std::cerr << it->first << ": ";
        it->second->help( std::cout);
    }
    
    exit(0);
}

prefix_ void Configuration::version()
{
    std::cerr << "0.1" << std::endl;
    exit(0);
}

prefix_ void Configuration::enableHighresTimers()
{
    // switching to scheduler highres timer
    if (senf::scheduler::haveScalableHiresTimers())
        SENF_LOG((senf::log::IMPORTANT)("using 'timerfd' hi-res timer."));
    else
        SENF_LOG((senf::log::IMPORTANT)("using 'posix' hi-res timer."));
    senf::scheduler::hiresTimers();
}

prefix_ void Configuration::enableRealtimeScheduling()
{
    // switching to kernel realtime scheduling
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &param) !=0 ) {
        SENF_LOG((senf::log::IMPORTANT)("WARNING: unable to switch to realtime mode "));
    } else {
        mlockall(MCL_FUTURE);
        SENF_LOG((senf::log::IMPORTANT) ("switching to realtime mode (SCHED_FIFO)"));
    }
}

prefix_ bool Configuration::parse( int argc, char const *argv[])
{
    namespace fty = senf::console::factory;
    namespace kw = senf::console::kw;
    senf::console::DirectoryNode & initDir (senf::console::root()["init"]);

    senf::console::ProgramOptions cmdlineOptions ( argc, argv, initDir);

    try {
        // now we parse the filled up configBundle
        cmdlineOptions.parse( initDir);
    } catch (senf::console::SyntaxErrorException const & e) {
        std::cerr << e.message() << std::endl;
        return false;
    }
    catch (senf::ExceptionMixin & ex) {
        std::cerr << "Exception occurred during configuration parsing: "
                  << ex.message() << std::endl
                  << ex.backtrace() << std::endl;
        return false;
    }

    return true;
}
