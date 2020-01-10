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
