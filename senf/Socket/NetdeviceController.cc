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
    \brief NetdeviceController non-inline non-template implementation
 */

#include "NetdeviceController.hh"
//#include "NetdeviceController.ih"

// Custom includes
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_vlan.h>
#include <linux/net_tstamp.h>
#include <boost/weak_ptr.hpp>
#include <boost/filesystem/fstream.hpp>
#include <senf/Utils/Exception.hh>
#include <senf/Utils/String.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define doIoctl(ifr, request, errorMsg)                                                         \
    if ( ::ioctl( sockfd_->fd, request, &ifr ) < 0 )                                            \
        SENF_THROW_SYSTEM_EXCEPTION("NetdeviceController: " errorMsg)


prefix_ senf::NetdeviceController::NetdeviceController(std::string const & interface_name)
    : sockfd_ (sockfd())
{
    reset(interface_name);
}

prefix_ senf::NetdeviceController::NetdeviceController(int interface_index)
    : sockfd_ (sockfd())
{
    ifindex_ = interface_index;
}

prefix_ void senf::NetdeviceController::reset(std::string const & interface_name)
{
    struct ifreq ifr;
    ::memset( &ifr, 0, sizeof(ifr));
    interface_name.copy( ifr.ifr_name, IFNAMSIZ);
    doIoctl(ifr, SIOCGIFINDEX, "Could not discover the index of interface \"" + interface_name + "\"");
    ifindex_ = ifr.ifr_ifindex;
}

prefix_ std::string senf::NetdeviceController::interfaceName()
    const
{
    try {
        struct ifreq ifr;
        ifrName( ifr);
        return std::string( ifr.ifr_name);
    }
    catch(...) {
        return "(interface not known)";
    }
}

prefix_ void senf::NetdeviceController::interfaceName(std::string const & newname)
{
    if (sizeof(newname) <= IFNAMSIZ) {
        struct ifreq ifr;
        ifrName(ifr);
        newname.copy(ifr.ifr_newname, IFNAMSIZ);
        doIoctl(ifr, SIOCSIFNAME, "Could not change the interface name. Is the interface really down?");
    }
    return;
}

prefix_ senf::MACAddress senf::NetdeviceController::hardwareAddress()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFHWADDR, "Could not discover hardwareAddress");
    return MACAddress::from_data( ifr.ifr_hwaddr.sa_data);
}

prefix_ void senf::NetdeviceController::hardwareAddress(MACAddress const & newAddress) {
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_hwaddr.sa_family = 1; // TODO: lookup named constant; PF_LOCAL ???
    std::copy(newAddress.begin(), newAddress.end(), ifr.ifr_hwaddr.sa_data);
    doIoctl(ifr, SIOCSIFHWADDR, "Could not change the interface MAC address. Is the interface really down?");
}

prefix_ int senf::NetdeviceController::mtu()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFMTU, "Could not discover mtu");
    return ifr.ifr_mtu;
}

prefix_ void senf::NetdeviceController::mtu(int new_mtu)
{
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_mtu = new_mtu;
    doIoctl( ifr, SIOCSIFMTU, "Could not set mtu");
}

prefix_ int senf::NetdeviceController::txqueuelen()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFTXQLEN, "Could not discover txqueuelen");
    return ifr.ifr_qlen;
}

prefix_ void senf::NetdeviceController::txqueuelen(int new_txqueuelen)
{
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_qlen = new_txqueuelen;
    doIoctl( ifr, SIOCSIFTXQLEN, "Could not set txqueuelen");
}


prefix_ bool senf::NetdeviceController::promisc()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFFLAGS, "Could not discover promisc mode");
    return ifr.ifr_flags & IFF_PROMISC;
}

prefix_ void senf::NetdeviceController::promisc(bool mode)
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFFLAGS, "Could not set promisc mode");
    if (mode)
        ifr.ifr_flags |= IFF_PROMISC;
    else
        ifr.ifr_flags &= ~IFF_PROMISC;
    doIoctl( ifr, SIOCSIFFLAGS, "Could not set promisc mode");
}

prefix_ bool senf::NetdeviceController::isUp()
    const
{
    struct ifreq ifr;
    ifrName(ifr);
    doIoctl(ifr, SIOCGIFFLAGS, "Could not discover interface status");
    return ifr.ifr_flags & IFF_UP;
}

prefix_ void senf::NetdeviceController::up()
{
    struct ifreq ifr;
    ifrName(ifr);
    doIoctl(ifr, SIOCGIFFLAGS, "Could not set interface status");
    ifr.ifr_flags |= IFF_UP;
    doIoctl(ifr, SIOCSIFFLAGS, "Could not set interface status");
}

prefix_ void senf::NetdeviceController::down()
{
    struct ifreq ifr;
    ifrName(ifr);
    doIoctl(ifr, SIOCGIFFLAGS, "Could not set interface status");
    ifr.ifr_flags &= ~IFF_UP;
    doIoctl(ifr, SIOCSIFFLAGS, "Could not set interface status");
}

prefix_ void senf::NetdeviceController::addVLAN(std::uint16_t vlanId)
{
    vlan_ioctl_args vlan_request;
    memset(&vlan_request, 0, sizeof(vlan_request));
    vlan_request.cmd     = ADD_VLAN_CMD;
    vlan_request.u.VID   = vlanId;
    strncpy(vlan_request.device1, interfaceName().c_str(), sizeof(vlan_request.device1)-1);
    doIoctl (vlan_request, SIOCSIFVLAN, "failed to add VLanId " + senf::str(vlanId) + " to " + interfaceName());
}

prefix_ void senf::NetdeviceController::delVLAN(std::uint16_t vlanId)
{
    vlan_ioctl_args vlan_request;
    memset(&vlan_request, 0, sizeof(vlan_request));
    vlan_request.cmd     = DEL_VLAN_CMD;
    strncpy(vlan_request.device1, (interfaceName() + "." + senf::str(vlanId)).c_str(), sizeof(vlan_request.device1)-1);
    doIoctl (vlan_request, SIOCSIFVLAN, "failed to delete VLanId " + senf::str(vlanId) + " from " + interfaceName());
}

prefix_ void senf::NetdeviceController::timestamping(int txType, int rxFilter)
{
    struct hwtstamp_config hwconf = { 0, txType, rxFilter};
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_data = (char*) &hwconf;
    doIoctl( ifr, SIOCSHWTSTAMP, "Failed to set hw timestamp config");
}

prefix_ int senf::NetdeviceController::interfaceIndex()
    const
{
    return ifindex_;
}

prefix_ void senf::NetdeviceController::ifrName(ifreq & ifr)
    const
{
    ::memset( &ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = ifindex_;
    if ( ::ioctl( sockfd_->fd, SIOCGIFNAME, &ifr ) < 0 )
        SENF_THROW_SYSTEM_EXCEPTION("NetdeviceController")
        << " could not discover the name of the interface with index " << ifindex_ << ".";
}

#undef doIoctl


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::NetdeviceController::SockFd

prefix_ senf::NetdeviceController::SockFd::SockFd()
    : fd (::socket(PF_INET, SOCK_DGRAM, 0))
{
    if ( fd < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not open socket for NetdeviceController.");
}

prefix_ senf::NetdeviceController::SockFd::~SockFd()
{
    ::close(fd);
}

prefix_ senf::NetdeviceController::SockFd::ptr senf::NetdeviceController::sockfd()
{
    static boost::weak_ptr<SockFd> sockfd;
    SockFd::ptr p (sockfd.lock());
    if (!p)
         sockfd = p = SockFd::ptr(new SockFd());
    return p;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::NetdeviceController::mactoName()

prefix_ senf::MACAddress senf::NetdeviceController::readMACAddressFromFile(boost::filesystem::path const & path)
{
    boost::filesystem::ifstream file;
    file.exceptions( std::ifstream::failbit | std::ifstream::badbit);
    file.open(path, std::ios::in);
    senf::MACAddress addr;
    file >> addr;
    file.close();
    return addr;
}

prefix_ std::string senf::NetdeviceController::macToName(senf::MACAddress const & mac)
{
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr ("/sys/class/net/"); itr != end_itr; ++itr) {
        try {
            if (readMACAddressFromFile(itr->path()/"address") == mac)
                return itr->path().filename().string();
        } catch (std::exception &) {}
    }
    return "";
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "NetdeviceController.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
