// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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
///////////////////////////////cc.p////////////////////////////////////////

prefix_ std::string senf::TapSocketProtocol::init_client()
    const
{
    return init_client(std::string());
}

prefix_ std::string senf::TapSocketProtocol::init_client(std::string const & interface_name, bool const NO_PI)
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
    if (::ioctl(f, TUNSETIFF, (void *) &ifr) < 0 )
        SENF_THROW_SYSTEM_EXCEPTION( "Could not create tap device: ") << ifr.ifr_name << ".";
    ifaceIndex_ = if_nametoindex(ifr.ifr_name);
    fd(f);
    return ifaceName();
}

prefix_ unsigned senf::TapSocketProtocol::available()
  const
{
  if (! fh().readable())
      return 0;
  ssize_t l = ::recv(fd(),0,0,MSG_PEEK | MSG_TRUNC);
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

///////////////////////////////cc.e////////////////////////////////////////
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
