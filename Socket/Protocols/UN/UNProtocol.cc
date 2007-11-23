// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum NETwork research (NET)
//     David Wagner <david.wagner@fokus.fraunhofer.de>
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
    \brief UNProtocol non-inline non-template implementation */

#include "UNProtocol.hh"
//#include "UNProtocol.ih"

// Custom includes
#include <fstream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include "../../../Utils/Exception.hh"

//#include "UNProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
prefix_ unsigned senf::UNProtocol::available()
    const
{
    int n;
    if (::ioctl(body().fd(),SIOCINQ,&n) < 0)
        throwErrno();
    return n;
}

prefix_ bool senf::UNProtocol::eof()
    const
{
    return false;
}

prefix_ void senf::UNProtocol::connect(UNSocketAddress const & address) 
    const 
{
    if(::connect(body().fd(), address.sockaddr_p(), sizeof(sockaddr_un)) < 0)
        throwErrno();
}

prefix_ void senf::UNProtocol::bind(UNSocketAddress const & address) 
    const 
{
    if(::bind(body().fd(), address.sockaddr_p(), sizeof(sockaddr_un)) < 0)
        throwErrno();
    
}



prefix_ void senf::UNProtocol::close() 
    const
{
    check_and_unlink();
  
    SocketProtocol::close();
}

prefix_ void senf::UNProtocol::terminate() 
    const
{
    check_and_unlink();
    
    SocketProtocol::terminate();
}

prefix_ void senf::UNProtocol::check_and_unlink()
    const
{
//  struct sockaddr_un test;
//  socklen_t len;
//  memset( (char*)&test, 0xff, sizeof( test));
//  int fd = inputSocket.fd() ;
////    printf( "fd: %d\n", fd);
//
//  int r = getsockname( fd, (struct sockaddr *)&test, &len);
//  if( r < 0){
//    perror( "bla:");
//  }
//  else{
//    printf( "name: %d %d %s\n", r, len , test.sun_path);
//    unsigned char *p = (unsigned char*) &test;for( r=0; r< len; r++) printf( "%2.2x ", (int)(p[r])); printf ("\n");
//  }
    struct sockaddr_un test;
    socklen_t len = sizeof( test);
    int r = ::getsockname( body().fd(), (struct sockaddr *)&test, &len);
    if( r == 0 && ::strlen(test.sun_path) > 0){
      ::unlink( test.sun_path);
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UNProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
