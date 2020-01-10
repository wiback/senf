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
    \brief Config non-inline non-template implementation */

#include "Config.hh"
//#include "Config.ih"

// Custom includes
#include <unistd.h>
#include <senf/Ext/NetEmu/WLAN/EmulatedWLANInterface.hh>
#include <senf/Ext/NetEmu/WLAN/HardwareWLANInterface.hh>

//#include "Config.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// wiback::Config

prefix_ wiback::Config::Config()
    : dir (this),
      probeInterval_ (senf::ClockService::milliseconds(1000)),
      quiet_ (false),
      silent_ (false),
      maxPacketDelay_ (senf::ClockService::milliseconds(10)),
      maxSchedulingDelay_ (senf::ClockService::milliseconds(10)),
      burnCycles_ (0),
      numPackets_ (1),
      consolePort_ (0u)
{
    namespace kw = senf::console::kw;
    namespace fty = senf::console::factory;

    dir.add("probe-interval",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         probeInterval, (senf::ClockService::clock_type)))
            .doc("Set's the interval at which packets are sent")
            .arg("interval", "packet send interval", kw::parser=senf::parseClockServiceInterval) );
    dir.add("probe-interval",
            fty::Command(SENF_MEMBINDFNP(senf::ClockService::clock_type, Config,
                                         probeInterval, () const ))
            .doc("Get's the interval at which helopackets are sent") );
    dir.add("quiet",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         quiet, (bool)))
            .doc("Disables detailed logging of packets sent/received.")
            .arg("quiet", "quiet flag", kw::default_value=true) );
    dir.add("get_quiet",
            fty::Command(SENF_MEMBINDFNP(bool, Config,
                                         quiet, () const ))
            .doc("Show if detailed logging of packets sent/received is activated.") );
    dir.add("silent",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         silent, (bool)))
            .doc("Disables all log messages except summary information at program termination.")
            .arg("silent", "silent flag", kw::default_value=true) );
    dir.add("get_silent",
            fty::Command(SENF_MEMBINDFNP(bool, Config,
                                         silent, () const ))
            .doc("Show status of silent.") );
    dir.add("max-scheduling-delay",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         maxSchedulingDelay, (senf::ClockService::clock_type)))
            .doc("Report a 'scheduling delay' incident if a timer tick is delayed by more than"
                 "this.")
            .arg("delay", "max delay time", kw::parser=senf::parseClockServiceInterval) );
    dir.add("max-scheduling-delay",
            fty::Command(SENF_MEMBINDFNP(senf::ClockService::clock_type, Config,
                                         maxSchedulingDelay, () const ))
            .doc("Get the value of the max-scheduling-delay") );
    dir.add("max-packet-delay",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         maxPacketDelay, (senf::ClockService::clock_type)))
            .doc("Report a delayed packet if a packet is older than this.")
            .arg("delay", "max delay time", kw::parser=senf::parseClockServiceInterval) );
    dir.add("max-packet-delay",
            fty::Command(SENF_MEMBINDFNP(senf::ClockService::clock_type, Config,
                                         maxPacketDelay, () const))
            .doc("Get the value of the max-packet-delay") );
    dir.add("burn-cycles",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         burnCycles, (unsigned)))
            .doc("Number of iterations of an explicit idle loop performed for each received\n"
                 "packet. Used for profiling.")
            .arg("n", "number of loop repetitions") );
    dir.add("burn-cycles",
            fty::Command(SENF_MEMBINDFNP(unsigned, Config,
                                         burnCycles, () const ))
            .doc("Show the number of iterations of an explicit idle loop performed for each\n"
                 "received packet. Used for profiling.") );
    dir.add("num-packets",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         numPackets, (unsigned)))
            .doc("Number of packets to sent per probe-interval tick.")
            .arg("n", "number of packets / tick") );
    dir.add("num-packets",
            fty::Command(SENF_MEMBINDFNP(unsigned, Config,
                                         numPackets, () const ))
            .doc("Show number of packets to sent per probe-interval tick.") );
    dir.add("console-port",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         consolePort, (unsigned)))
            .doc("Set port for interactive control console")
            .arg("port","port number") );
    dir.add("console-port",
            fty::Command(SENF_MEMBINDFNP(unsigned, Config,
                                         consolePort, () const ))
            .doc("Get port for interactive control console") );
    dir.add("nodeCoordinates",
            fty::Command(SENF_MEMBINDFNP(void, Config,
                                         nodeCoordinates, (unsigned,double,double)))
            .doc("Define node coordinates")
            .arg("id", "Node id")
            .arg("latitude", "GPS latitude of node 'id'")
            .arg("longitude", "GPS longitude of node 'id'") );

    dir.add("help",
            fty::Command(&Config::showHelp, this)
            .doc("Show help.")
            .arg("option", "name of option", kw::default_value="") );
}

prefix_ void wiback::Config::showHelp(std::ostream & os, std::string const & command)
{
    if (command.empty()) {
        std::cerr << "Available options:\n\n";
        senf::console::DirectoryNode::ChildrenRange cld (dir.children());
        for (senf::console::DirectoryNode::ChildrenRange::iterator i (cld.begin());
             i != cld.end(); ++i)
            std::cerr << "--" << i->first << "\n";
        std::cerr << "\nUse --help=<option> for more information.\n";
    }
    else {
        dir.node().get(command).help(std::cerr);
    }
    ::exit(0);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Config.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U"
// End:
