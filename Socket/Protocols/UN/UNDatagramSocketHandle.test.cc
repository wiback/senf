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
    \brief UNDatagramSocketHandle.test unit tests */

//#include "UNDatagramSocketHandle.test.hh"
//#include "UNDatagramSocketHandle.test.ih"

// Custom includes
#include "UNDatagramSocketHandle.hh"

#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(unDatagramSocketHandle)
{
  std::string hallo = "Hallo Welt.";
  std::string testS = ".socket-UNDatagramSocketHandle.test";
  
  unlink(testS.c_str()); 

  senf::UNSocketAddress addr (testS) ; 
  senf::UNDatagramClientSocketHandle inputSocket(addr);
  senf::UNDatagramClientSocketHandle outputSocket;

  outputSocket.writeto( addr, hallo);

  BOOST_CHECK_EQUAL( inputSocket.read(), hallo);
  outputSocket.close();
  inputSocket.close();
  
  if( unlink(testS.c_str()) != 0) 
       perror( "unlink failed");
}


///////////////////////////////cc.e////////////////////////////////////////
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
