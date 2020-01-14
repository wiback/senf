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
    \brief FileTarget unit tests */

#include "FileTarget.hh"

// Custom includes
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include "Logger.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(fileTarget)
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
