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
    \brief CPU Stat public header */

#ifndef HH_SENF_Utils_CpuStat_
#define HH_SENF_Utils_CpuStat_ 1

// Custom includes
#include <list>
#include <senf/Utils/singleton.hh>
#include <senf/Scheduler/ClockService.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    struct CpuStat
    {
        ClockService::clock_type duration;
        unsigned user;
        unsigned system;
        unsigned nice;
        unsigned idle;
        unsigned iowait;
        unsigned irq;
        unsigned softirq;
        unsigned steal;
        unsigned guest;

        CpuStat( ClockService::clock_type duration = ClockService::milliseconds( 0),
                 unsigned user=0,
                 unsigned system=0,
                 unsigned nice=0,
                 unsigned idle=0,
                 unsigned iowait=0,
                 unsigned irq=0,
                 unsigned softirq=0,
                 unsigned steal=0,
                 unsigned guest=0);
    };

    class CpuStatProb
    {
    public:
        CpuStat cpuStat();

    private:
        typedef std::vector<boost::long_long_type> CpuStatProbs;
        CpuStatProbs probs;
        ClockService::clock_type time;
    };


    class CpuStatConsole
    {
    public:
        CpuStatConsole();
        void dump(std::ostream & os);

    private:
        CpuStatProb procStats_;
    };

    std::ostream & operator<<(std::ostream & os, CpuStat const & cs);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "CpuStat.cci"
//#include "CpuStat.ct"
//#include "CpuStat.cti"
#endif
