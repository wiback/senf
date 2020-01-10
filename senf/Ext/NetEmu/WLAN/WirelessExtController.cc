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
    \brief WirelessExtController non-inline non-template implementation */

#include "WirelessExtController.hh"

// Custom includes
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <math.h>
extern "C" {
#include <linux/if.h>
#include <linux/wireless.h>
}
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Utils/Exception.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WirelessExtController

prefix_ senf::emu::WirelessExtController::WirelessExtController(std::string const & ifname)
    : ifname_ (ifname), sockfd_ (NetdeviceController::sockfd())
{}

prefix_ senf::emu::WirelessExtController::WirelessExtController(unsigned ifindex)
    : ifname_ (NetdeviceController(ifindex).interfaceName()),
      sockfd_ (NetdeviceController::sockfd())
{}

prefix_ senf::emu::WirelessExtController::frequency_type senf::emu::WirelessExtController::frequency()
    const
{
    struct iwreq req;
    bzero(&req, sizeof(req));
    doIoctl(SIOCGIWFREQ, &req);
    return (((double)req.u.freq.m) * pow(10,req.u.freq.e)) / 1000;
}

prefix_ std::string senf::emu::WirelessExtController::ssid()
    const
{
    struct iwreq req;
    bzero(&req, sizeof(req));
    char essid[IW_ESSID_MAX_SIZE + 1];
    req.u.essid.pointer = (caddr_t)essid;
    req.u.essid.length = IW_ESSID_MAX_SIZE;
    req.u.essid.flags = 0;
    doIoctl(SIOCGIWESSID, &req);
    essid[req.u.essid.length] = 0;
    return std::string(essid);
}

prefix_ senf::MACAddress senf::emu::WirelessExtController::cellId()
    const
{
    struct iwreq req;
    bzero(&req, sizeof(req));
    doIoctl(SIOCGIWAP, &req);
    return senf::MACAddress::from_data(req.u.ap_addr.sa_data);
}

prefix_ void senf::emu::WirelessExtController::doIoctl(int ctl, struct iwreq * data)
    const
{
    strncpy(data->ifr_name, ifname_.c_str(), IFNAMSIZ-1);
    data->ifr_name[IFNAMSIZ-1] = 0;
    if (ioctl(sockfd_->fd, ctl, data) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("ioctl()");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
