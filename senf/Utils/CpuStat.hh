// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Jens Moedeker <jmo@berlios.de>

/** \file
    \brief CPU Stat public header */

#ifndef HH_SENF_Scheduler_CpuStat_
#define HH_SENF_Scheduler_CpuStat_ 1

// Custom includes
#include <list>
#include <senf/Utils/singleton.hh>
#include <senf/Scheduler/ClockService.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    struct CpuStat{
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

    class CpuStatProb{
    public:
        CpuStat cpuStat();

    private:
        typedef std::vector<boost::long_long_type> CpuStatProbs;
        CpuStatProbs probs;
        ClockService::clock_type time;

        void readProcStat( CpuStatProb &);
    };


    class CpuStatConsole
    {
    public:

        void dump( std::ostream & os);

        CpuStatConsole();

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
