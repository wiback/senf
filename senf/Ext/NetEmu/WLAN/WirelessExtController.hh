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
    \brief WirelessExtController public header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_WirelessExtController_
#define HH_SENF_Ext_NetEmu_WLAN_WirelessExtController_ 1

// Custom includes
#include <senf/Socket/NetdeviceController.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

struct iwreq;

namespace senf {
namespace emu {

    class WirelessExtController
    {
    public:
        typedef unsigned frequency_type;

        explicit WirelessExtController(std::string const & ifname);
        explicit WirelessExtController(unsigned ifindex);

        frequency_type frequency() const;
        std::string ssid() const;
        senf::MACAddress cellId() const;

    private:
        void doIoctl(int ctl, struct iwreq * data) const;
        void getrange() const;

        std::string ifname_;
        NetdeviceController::SockFd::ptr sockfd_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "WLANController.cci"
//#include "WLANController.ct"
//#include "WLANController.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
