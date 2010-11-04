// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief FileTarget unit tests */

//#include "FileTarget.test.hh"
//#include "FileTarget.test.ih"

// Custom includes
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include "FileTarget.hh"
#include "Logger.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(fileTarget)
{
    std::string filename ("/tmp/senf_fileTarget_test.log");
    senf::log::FileTarget target (filename);

    target.tag("");
    target.showTime(false);
    target.showLevel(false);
    target.route();

    std::string message ("Test message");
    SENF_LOG((message));

    BOOST_REQUIRE( boost::filesystem::exists(filename));
    std::ifstream inFile;
    inFile.open(filename.c_str());
    BOOST_REQUIRE( inFile);

    char buffer[256];
    inFile.getline(buffer, 256);
    inFile.close();
    BOOST_CHECK_EQUAL( std::string(buffer), message);

    SENF_CHECK_NO_THROW( boost::filesystem::remove(filename) );

    target.reopen();
    BOOST_REQUIRE( boost::filesystem::exists(filename));
    SENF_CHECK_NO_THROW( boost::filesystem::remove(filename) );

    filename = std::string("/tmp/senf_fileTarget_test2.log");
    target.reopen( filename);
    BOOST_REQUIRE( boost::filesystem::exists(filename));
    SENF_CHECK_NO_THROW( boost::filesystem::remove(filename) );
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
