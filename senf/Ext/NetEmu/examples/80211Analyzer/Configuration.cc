// $Id:$
//
// Copyright (C) 2012
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
//     Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

#include "Configuration.hh"

// Custom includes
extern "C" {
  #include <sys/mman.h>
}
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Console.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ Configuration::Configuration()
    : mgenMode(false),
      iperfMode(false),
      bindSocket(true),
      tsftHistogram(false),
      verbose(false),
      csvMode(false),
      reorderPackets(true),
      promisc(false),
      clockDrift(0.0f),
      reorderTimeout(senf::ClockService::milliseconds(0)),  // NetEmu default
      reportingInterval(senf::ClockService::milliseconds(1000)),
      duration(senf::ClockService::milliseconds(0)),
      maxWait(senf::ClockService::milliseconds(0)),
      device("mon0"),
      monitorMode(true),
      analyzeTx(false),
      destination(5001),
      numPackets(0)
{
    namespace fty = senf::console::factory;
    senf::console::DirectoryNode & initDir (senf::console::root().add("init", fty::Directory()));
    initDir.add("help", fty::Command( &Configuration::help, this));
    initDir.add("version", fty::Command( &Configuration::version, this));
    initDir.add("realtime-scheduling", fty::Command( &Configuration::enableRealtimeScheduling, this));

    initDir.add("source", fty::Variable( source));
    initDir.add("destination", fty::Variable( destination));
    initDir.add("log-destination", fty::Variable( logDestination));
    initDir.add("mgen-mode", fty::Variable( mgenMode));
    initDir.add("iperf-mode", fty::Variable( iperfMode));
    initDir.add("bind-socket", fty::Variable( bindSocket));
    initDir.add("tsft-histogram", fty::Variable( tsftHistogram));
    initDir.add("verbose", fty::Variable( verbose));
    initDir.add("csv-mode", fty::Variable( csvMode));
    initDir.add("clockDrift", fty::Variable( clockDrift)
        .doc("Clock Drift between the nodes in ms/s (in MGEN mode). Defaults to 0."));
    initDir.add("reporting-interval", fty::Variable( reportingInterval)
        .parser(senf::parseClockServiceInterval) );
    initDir.add("duration", fty::Variable( duration)
        .parser(senf::parseClockServiceInterval) );
    initDir.add("max-wait", fty::Variable( maxWait)
        .parser(senf::parseClockServiceInterval) );
    initDir.add("num-packets",  fty::Variable( numPackets));
    initDir.add("monitor-device", fty::Command( &Configuration::monDev, this));
    initDir.add("device", fty::Command( &Configuration::dev, this));
    initDir.add("analyze-tx", fty::Variable(analyzeTx));
    initDir.add("reorder-packets", fty::Variable( reorderPackets));
    initDir.add("reorder-timeout", fty::Variable( reorderTimeout)
        .parser(senf::parseClockServiceInterval) );
    initDir.add("promisc", fty::Variable( promisc));

    // always turn those on, where available
    enableHighresTimers();
}

prefix_ void Configuration::help()
{
    senf::console::DirectoryNode::ChildrenRange range (senf::console::root()["init"].children());

    senf::console::DirectoryNode::ChildrenRange::iterator it;
    for (it = range.begin(); it != range.end(); it++) {
        std::cerr << it->first << ": ";
        it->second->help( std::cout);
    }

    exit(0);
}

prefix_ void Configuration::version()
{
    std::cerr << "2.0" << std::endl;
    exit(0);
}

prefix_ void Configuration::dev(std::string const & d)
{
    device = d;
    monitorMode = false;
}

prefix_ void Configuration::monDev(std::string const & d)
{
    device = d;
    monitorMode = true;
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
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        SENF_LOG((senf::log::IMPORTANT)("WARNING: unable to switch to realtime mode "));
    } else {
        mlockall(MCL_FUTURE);
        SENF_LOG((senf::log::IMPORTANT) ("switching to realtime mode (SCHED_FIFO)"));
    }
}

prefix_ bool Configuration::parse(int argc, char const *argv[])
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
