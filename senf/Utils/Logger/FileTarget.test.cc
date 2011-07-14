// $Id$
//
// Copyright (C) 2009
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
