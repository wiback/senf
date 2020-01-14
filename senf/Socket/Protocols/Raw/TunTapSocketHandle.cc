//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief

 */

#include "TunTapSocketHandle.hh"
//#include "TunTapSocketHandle.ih"

// Custom includes
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <errno.h>

//#include "TunTapSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::TapSocketProtocol::init_client()
    const
{
    init_client(std::string());
}

prefix_ void senf::TapSocketProtocol::init_client(std::string const & interface_name, bool NO_PI)
    const
{
    int f;
    if ( (f = ::open("/dev/net/tun", O_RDWR  | O_CLOEXEC)) < 0 )
        SENF_THROW_SYSTEM_EXCEPTION("Could not open tap control device: /dev/net/tun.");
    struct ifreq ifr;
    ::memset( &ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP;
    if (NO_PI)
        ifr.ifr_flags |= IFF_NO_PI;
    interface_name.copy( ifr.ifr_name, IFNAMSIZ);
    if (::ioctl(f, TUNSETIFF, (void *) &ifr) < 0 ) {
        ::close(f);
        SENF_THROW_SYSTEM_EXCEPTION( "Could not create tap device: ") << ifr.ifr_name << ".";
    }
    ifaceIndex_ = if_nametoindex(ifr.ifr_name);
    if (ifaceIndex_ == 0)
        SENF_THROW_SYSTEM_EXCEPTION( "if_nametoindex(): failed to access new tap device '" )
            << ifr.ifr_name << "'.";
    fd(f);
}

prefix_ unsigned senf::TapSocketProtocol::available()
  const
{
    if (!fh().readable())
        return 0;
    ssize_t l = ::recv(fd(), 0, 0, MSG_PEEK | MSG_TRUNC);
    if (l < 0)
        //SENF_THROW_SYSTEM_EXCEPTION("");
        return 1588;
    return l;
}

/*
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ

prefix_ unsigned senf::TapSocketProtocol::available()
  const
{
  if (! body().readable())
      return 0;
  int n;
  if (::ioctl(body().fd(),SIOCINQ,&n) < 0)
      SENF_THROW_SYSTEM_EXCEPTION("");
  return n;
}
*/

prefix_ bool senf::TapSocketProtocol::eof()
    const
{
    return false;
}

prefix_ unsigned int senf::TapSocketProtocol::ifaceIndex()
    const
{
    return ifaceIndex_;
}

prefix_ std::string senf::TapSocketProtocol::ifaceName()
    const
{
    char buf[IF_NAMESIZE];
    if_indextoname(ifaceIndex_, buf);
    return std::string(buf);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "TunTapSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
