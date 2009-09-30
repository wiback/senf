// $Id$
//
// Copyright (C) 2008 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief Statistics non-inline non-template implementation */

#include "Statistics.hh"
//#include "Statistics.ih"

// Custom includes
#include <cmath>
#include <sstream>
#include <senf/Utils/Console/Console.hh>
#include <senf/Utils/Format.hh>
#include "StatisticsTargets.hh"

//#include "Statistics.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::StatisticsBase

prefix_ void senf::StatisticsBase::enter(float min, float avg, float max, float dev)
{
    min_ = min;
    avg_ = avg;
    max_ = max;
    dev_ = dev;
    generateOutput();
    signalChildren();
}

prefix_ senf::Collector & senf::StatisticsBase::operator[](unsigned rank)
{
    Children::iterator i (children_.find(rank));
    if (i == children_.end())
        throw InvalidRankException();
    return i->second;
}

prefix_ senf::Collector & senf::StatisticsBase::collect(unsigned rank)
{
    std::pair<Children::iterator, bool> state (
        children_.insert(std::make_pair(rank, Collector(this, rank))) );
    if (! state.second)
        throw DuplicateRankException();
    return state.first->second;
}

prefix_ senf::StatisticsBase::OutputProxy<senf::StatisticsBase>
senf::StatisticsBase::output(unsigned n)
{
    OutputMap::iterator i (outputs_.find(n));
    if (i == outputs_.end()) {
        i = outputs_.insert(std::make_pair(n, OutputEntry(n))).first;
        std::stringstream nm;
        nm << "output" << path() << ":" << n;
        base().dir.node().add(nm.str(), i->second.dir);
        detail::StatisticsLoggerRegistry::instance().apply(*this, n, i->second.dir);
    }
    if (n > maxQueueLen_)
        maxQueueLen_ = n;
    return OutputProxy<StatisticsBase>(this, &(i->second));
}

prefix_ void senf::StatisticsBase::consoleList(unsigned level, std::ostream & os)
    const
{
    namespace fmt = senf::format;

    os << boost::format("%s%-5d%|15t|  %12.5g  %19.5g  %12.5g\n") 
        % std::string(2*level,' ') % rank() 
        % fmt::eng(min()).setw() % fmt::eng(avg(),dev()).setw() % fmt::eng(max()).setw();
    {
        OutputMap::const_iterator i (outputs_.begin());
        OutputMap::const_iterator i_end (outputs_.end());
        for (; i != i_end; ++i)
            os << boost::format("            %3d  %12.5g  %19.5g  %12.5g\n")
                % i->second.n 
                % fmt::eng(i->second.min).setw() 
                % fmt::eng(i->second.avg, i->second.dev).setw() 
                % fmt::eng(i->second.max).setw();
    }
    {
        Children::const_iterator i (children_.begin());
        Children::const_iterator const i_end (children_.end());
        for (; i != i_end; ++i)
            i->second.consoleList(level+1, os);
    }
}

prefix_ void senf::StatisticsBase::generateOutput()
{
    queue_.push_front(QueueEntry(min_, avg_, max_, dev_));
    while (queue_.size() > maxQueueLen_)
        queue_.pop_back();

    OutputMap::iterator i (outputs_.begin());
    OutputMap::iterator const i_end (outputs_.end());
    for (; i != i_end; ++i) {
        i->second.min = i->second.avg = i->second.max = i->second.dev = 0.0f;
        Queue::const_iterator j (queue_.begin());
        Queue::const_iterator const j_end (queue_.end());
        unsigned n (0);
        for (; n < i->second.n && j != j_end; ++n, ++j) {
            i->second.min += j->min;
            i->second.avg += j->avg;
            i->second.max += j->max;
            i->second.dev += j->dev;
        }
        i->second.min /= n;
        i->second.avg /= n;
        i->second.max /= n;
        i->second.dev /= n;
        i->second.signal(i->second.min, i->second.avg, i->second.max, i->second.dev);
    }
}

prefix_ void senf::StatisticsBase::signalChildren()
{
    Children::iterator i (children_.begin());
    Children::iterator const i_end  (children_.end());
    for (; i != i_end; ++i)
        i->second.enter(min_, avg_, max_, dev_);
}

///////////////////////////////////////////////////////////////////////////
// senf::Statistics

prefix_ senf::Statistics::Statistics()
#ifndef SENF_DISABLE_CONSOLE
    : dir (this)
#endif
{
#ifndef SENF_DISABLE_CONSOLE
    dir.add("list", &Statistics::consoleList)
        .doc("List statistics collection intervals and current values.\n"
             "\n"
             "Columns:\n"
             "    RANK    Number of values collected. Since the statistics collectors form\n"
             "            a tree, the value is indented according to it's tree location.\n"
             "    WIN     Size of output average window.\n"
             "    MIN     Last entered minimum value.\n"
             "    AVG     Last entered average value.\n"
             "    DEV     Standard deviation of average value over the collector rank.\n"
             "    MAX     Last entered maximum value.");
    dir.add("collect", &Statistics::consoleCollect)
        .doc("Add statistics collection groups. The argument gives a sequence of collector\n"
             "ranks each building on the preceding collector:\n"
             "\n"
             "    $ collect (10 60 60)\n"
             "\n"
             "Will start by collecting every 10 values together to a new value. 60 of such\n"
             "combined values will be collected together in the next step again followed by\n"
             "a collection of 60 values. If the statistics is entered with a frequency of\n"
             "10 values per second, this will provide combined statistics over the second,\n"
             "minutes and hours ranges.\n"
             "\n"
             "You may call collect multiple times. Any missing collection ranks will be\n"
             "added.")
        .arg("ranks","chain of collector ranks");
    dir.add("output", &Statistics::consoleOutput)
        .doc("Generate statistics output. This statement will add an additional output\n"
             "generator. This generator will be attached to the collector specified by\n"
             "the {rank} parameter. This parameter is a chain of successive rank values\n"
             "which specifies the exact collector to use. If the collector does not\n"
             "exist, it will be created (this is like automatically calling 'collect'\n"
             "with {rank} as argument).\n"
             "\n"
             "If the output is to be sent somewhere it must be connected to a statistics\n"
             "target.\n"
             "\n"
             "The output may optionally be built using a sliding average over the last\n"
             "{window} values.\n"
             "\n"
             "    $ output ()\n"
             "\n"
             "will output the basic statistics value each time a new value is entered.\n"
             "\n"
             "    $ output (10 60) 5\n"
             "\n"
             "Assuming that new data values are entered 10 times per second, this command\n"
             "will generate output once every minute. The value will be the average over\n"
             "the last 5 minutes.")
        .arg("rank","Rank chain selecting the value to generate output for")
        .arg("window","Optional size of sliding average window",
             senf::console::kw::default_value = 1u);
#endif
}

prefix_ void senf::Statistics::consoleList(std::ostream & os)
{
    os << "RANK        WIN       MIN          AVG                   MAX\n";
    StatisticsBase::consoleList(0, os);
}

prefix_ void senf::Statistics::consoleCollect(std::vector<unsigned> & ranks)
{
    StatisticsBase * stats (this);
    std::vector<unsigned>::const_iterator i (ranks.begin());
    std::vector<unsigned>::const_iterator const i_end (ranks.end());

    try {
        for (; i != i_end; ++i)
            stats = &(*stats)[*i];
    }
    catch (InvalidRankException &) {}

    for (; i != i_end; ++i)
        stats = & (stats->collect(*i));
        
}

prefix_  boost::shared_ptr<senf::console::DirectoryNode>
senf::Statistics::consoleOutput(std::vector<unsigned> & ranks, unsigned window)
{
    StatisticsBase * stats (this);
    std::vector<unsigned>::const_iterator i (ranks.begin());
    std::vector<unsigned>::const_iterator const i_end (ranks.end());
    
    try {
        for (; i != i_end; ++i)
            stats = &(*stats)[*i];
    }
    catch (InvalidRankException &) {}

    for (; i != i_end; ++i)
        stats = & (stats->collect(*i));
    
    return stats->output(window).dir().node().thisptr();
}

prefix_ senf::Statistics & senf::Statistics::v_base()
{
    return *this;
}

prefix_ std::string senf::Statistics::v_path()
    const
{
    return "";
}

///////////////////////////////////////////////////////////////////////////
// senf::Collector

prefix_ void senf::Collector::enter(float min, float avg, float max, float dev)
{
    accSum_ += avg;
    accSumSq_ += avg*avg + dev*dev;
    if (min < accMin_) accMin_ = min;
    if (max > accMax_) accMax_ = max;
    if (++i_ >= rank_) {
        float accAvg (accSum_ / i_);
        float accDev (std::max(0.0f,std::sqrt(accSumSq_ / i_ - accAvg*accAvg)));
        StatisticsBase::enter(accMin_, accAvg, accMax_, accDev);
        i_ = 0;
        accMin_ = FLT_MAX;
        accSum_ = 0.0f;
        accSumSq_ = 0.0f;
        accMax_ = -FLT_MAX;
    }
}

prefix_ senf::Statistics & senf::Collector::v_base()
{
    return owner_->base();
}

prefix_ std::string senf::Collector::v_path()
    const
{
    return owner_->path() + "-" + senf::str(rank_);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Statistics.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
