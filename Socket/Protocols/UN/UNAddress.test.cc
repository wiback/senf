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
    \brief UNAddress.test unit tests */

//#include "UNAddress.test.hh"
//#include "UNAddress.test.ih"

// Custom includes
#include "UNAddress.hh"
#include <boost/filesystem/path.hpp>
#include "../../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#include <iostream>
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(unAddress)
{
    // das koennt sicher mehr sein...
    std::string testS = "/tmp/senfTestSocket";
    boost::filesystem::path testp = boost::filesystem::path(testS); 
    senf::UNAddress addr1 = senf::UNAddress::fromString(testS);
    senf::UNAddress addr2 = senf::UNAddress::fromPath(testp);
    BOOST_CHECK( testS == addr1.pathString());
    BOOST_CHECK( testS == addr2.pathString());
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
