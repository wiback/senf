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
    : verbose(false),
      phyName("phy0"),
      debugFS("/sys/kernel/debug"),
      reportingInterval(senf::ClockService::milliseconds(1000)),
      duration(senf::ClockService::seconds(10)),
      frequency(5180),
      ht40(false),
      spectralPeriod(0x08),
      spectralFFTPeriod(0x02),
      spectralCount(0),
      spectralBins(64),
      spectralEndless(false),
      spectralShortRepeat(true)
{
    namespace fty = senf::console::factory;
    senf::console::DirectoryNode & initDir (senf::console::root().add("init", fty::Directory()));
    initDir.add("help", fty::Command(&Configuration::help, this));
    initDir.add("version", fty::Command(&Configuration::version, this));
    initDir.add("realtime-scheduling", fty::Command(&Configuration::enableRealtimeScheduling, this));

    initDir.add("verbose", fty::Variable(verbose));
    initDir.add("phy-name", fty::Variable(phyName));
    initDir.add("debug-fs", fty::Variable(debugFS));
    initDir.add("reporting-interval", fty::Variable( reportingInterval)
                .parser(senf::parseClockServiceInterval));
    initDir.add("duration", fty::Variable(duration)
                .parser(senf::parseClockServiceInterval));

    initDir.add("frequency", fty::Variable(frequency));
    initDir.add("ht40", fty::Variable(ht40));

    initDir.add("spectral-period", fty::Variable(spectralPeriod));
    initDir.add("spectral-fft-period", fty::Variable(spectralFFTPeriod));
    initDir.add("spectral-bins", fty::Variable(spectralBins));
    initDir.add("spectral-count", fty::Variable(spectralCount));
    initDir.add("spectral-endless", fty::Variable(spectralEndless));
    initDir.add("spectral-short-repeat", fty::Variable(spectralShortRepeat));

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
    std::cerr << "1.2" << std::endl;
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
