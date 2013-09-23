// $Id$
//
// Copyright (C) 2008
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
//   Thorsten Horstmann <tho@berlios.de>

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
    if ( (f = ::open("/dev/net/tun", O_RDWR)) < 0 )
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
