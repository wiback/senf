//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief CpuStat non-inline non-template implementation */

#include "CpuStat.hh"

// Custom includes
#include <fstream>
#include "Console/ScopedDirectory.hh"
#include "Console/Sysdir.hh"
#include "Console/ParsedCommand.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::CpuStat::CpuStat(ClockService::clock_type duration_, unsigned user_,
        unsigned system_, unsigned nice_, unsigned idle_, unsigned iowait_,
        unsigned irq_, unsigned softirq_, unsigned steal_, unsigned guest_)
    : duration(duration_), user(user_), system(system_), nice(nice_), idle(idle_),
      iowait(iowait_), irq(irq_), softirq(softirq_), steal(steal_), guest(guest_)
{
}

prefix_ senf::CpuStat senf::CpuStatProb::cpuStat()
{
    std::ifstream statfd ("/proc/stat");

    CpuStatProbs current;

    std::string prefix;
    statfd >> prefix;

    if (prefix.compare("cpu") != 0)
        return CpuStat();

    ClockService::clock_type now = ClockService::now();
    boost::long_long_type x;
    for (unsigned i=0; statfd.good() && statfd >> x; ++i) {
        current.push_back( x);
    }

    if (probs.size() == 0 || time == ClockService::seconds(0)) {
        probs = current;
        time = now;
        return CpuStat();
    }

    boost::long_long_type sum = 0;
    CpuStatProbs::iterator c = current.begin();
    CpuStatProbs::iterator p = probs.begin();
    for (; c != current.end() && p != probs.end(); ++c, ++p) {
        sum += *c - *p;
    }

    if (sum <= 0) {
        time = now;
        return CpuStat();
    }

    CpuStat cs (now - time,
            100 * (current[0] - probs[0]) / sum,
            100 * (current[1] - probs[1]) / sum,
            100 * (current[2] - probs[2]) / sum,
            100 * (current[3] - probs[3]) / sum,
            100 * (current[4] - probs[4]) / sum,
            100 * (current[5] - probs[5]) / sum,
            100 * (current[6] - probs[6]) / sum,
            100 * (current[7] - probs[7]) / sum,
            100 * (current[8] - probs[8]) / sum);

    probs = current;
    time = now;

    return cs;
}

prefix_ senf::CpuStatConsole::CpuStatConsole()
{
    namespace fty = senf::console::factory;

    console::sysdir().add("cpustat", fty::Command( &CpuStatConsole::dump, this))
      .doc("Returns the CPU usage based on /proc/stat since last call of this function");
}

prefix_ void senf::CpuStatConsole::dump(std::ostream & os)
{
    os << procStats_.cpuStat();
}


prefix_ std::ostream & senf::operator<<(std::ostream & os, CpuStat const & cs)
{
    boost::format fmtStat ("user=%3.0d%% sys=%3.0d%% nice=%3.0d%% idle=%3.0d%% io=%3.0d%% irq=%3.0d%% sirq=%3.0d%% steal=%3.0d%% guest=%3.0d%% duration=%dms ");
    os << fmtStat % cs.user % cs.system % cs.nice % cs.idle % cs.iowait % cs.irq % cs.softirq % cs.steal % cs.guest % ClockService::in_milliseconds(cs.duration) << std::endl;
    return os;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_

// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
