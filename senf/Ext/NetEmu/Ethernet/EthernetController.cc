// $Id:$
//
// Copyright (C) 2011
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
    \brief WLANController non-inline non-template implementation */

#include "EthernetController.hh"
//#include "EthernetController.ih"

// Custom includes
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <senf/Utils/Exception.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define doIoctl(ifr, errorMsg)                                                          \
    if ( ::ioctl( sockfd_->fd, SIOCETHTOOL, &ifr ) < 0 )                                \
        SENF_THROW_SYSTEM_EXCEPTION("EthernetController: " errorMsg)


prefix_ senf::emu::EthernetController::EthernetController(std::string const & interface_name)
    : NetdeviceController(interface_name)
{}

prefix_ senf::emu::EthernetController::EthernetController(int interface_index)
    : NetdeviceController(interface_index)
{}

prefix_ unsigned senf::emu::EthernetController::speed()
    const
{
    struct ifreq ifr;
    ifrName(ifr);
    struct ethtool_cmd edata;
    edata.cmd = ETHTOOL_GSET;
    ifr.ifr_data = reinterpret_cast<caddr_t>(&edata);
    doIoctl(ifr, "Could not discover speed of ethernet interface");
    return edata.speed;
}

prefix_ senf::emu::EthernetController::Duplex_mode senf::emu::EthernetController::duplex()
    const
{
    struct ifreq ifr;
    ifrName(ifr);
    struct ethtool_cmd edata;
    edata.cmd = ETHTOOL_GSET;
    ifr.ifr_data = reinterpret_cast<caddr_t>(&edata);
    doIoctl(ifr, "Could not discover duplex mode of ethernet interface");
    switch (edata.duplex) {
    case DUPLEX_HALF:
        return HALF_DUPLEX;
    case DUPLEX_FULL:
        return FULL_DUPLEX;
    case DUPLEX_UNKNOWN:
        return UNKNOWN_DUPLEX;
    default:
        SENF_THROW_SYSTEM_EXCEPTION("EthernetController: invalid duplex mode ")
            << "(" << unsigned(edata.duplex) << ")";
    }
}

prefix_ bool senf::emu::EthernetController::hasLink()
    const
{
    struct ifreq ifr;
    ifrName(ifr);
    struct ethtool_value edata;
    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = reinterpret_cast<caddr_t>(&edata);
    doIoctl(ifr, "Could not discover link status of ethernet interface");
    return edata.data;
}

#undef doIoctl

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
