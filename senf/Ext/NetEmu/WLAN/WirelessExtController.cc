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
// Contributor(s):
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief WirelessExtController non-inline non-template implementation */

#include "WirelessExtController.hh"

// Custom includes
#include <string.h>
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
    strncpy(data->ifr_name, ifname_.c_str(), IFNAMSIZ);
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
