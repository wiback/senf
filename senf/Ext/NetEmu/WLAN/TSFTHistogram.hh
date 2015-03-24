// $Id:$
//
// Copyright (C) 2009
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

/** \file
    \brief TSFT Histogram header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_TSFTHistogram_
#define HH_SENF_Ext_NetEmu_WLAN_TSFTHistogram_ 1

// Custom includes
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Scheduler/ClockService.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class TSFTHistogram 
    {
    public:
        senf::console::ScopedDirectory<> dir;

        TSFTHistogram();
        void update(senf::MACAddress const & src, boost::uint64_t const & tsft);
        bool start(senf::MACAddress const & src);
        bool start();
        bool enabled() const {
            return enabled_;
        }
        void dump(std::ostream & os) const;

    private:
        // we want a sorted map, even if it might be slightly slower - debugging only !
        typedef std::map<boost::uint32_t,boost::uint32_t> TSFTMap;
        TSFTMap tsftMap_;
        boost::uint64_t lastTSFT_;
        bool tsftMapInitialized_;
        senf::ClockService::clock_type startTime_;
        senf::ClockService::clock_type duration_;
        senf::MACAddress src_;
        bool enabled_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TSFTHistogram.cci"
//#include "TSFTHistogram.ct"
//#include "TSFTHistogram.cti"
#endif
