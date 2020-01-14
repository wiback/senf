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
    \brief Exception unit tests */

#include "Exception.hh"

// Custom includes
#include <boost/format.hpp>
#include <errno.h>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(wrapException)
{
    bool bad_cast (false);

    try {
        try {
            try {
                try {
                    try {
                        throw std::bad_cast();
                    }
                    SENF_WRAP_EXC(std::bad_cast)
                }
                SENF_WRAP_EXC(std::bad_cast)
            }
            catch (senf::ExceptionMixin & ex) {
                ex << "\nspecial exception";
                throw;
            }
        }
        catch (std::exception const & ex) {
#ifdef SENF_DEBUG
            BOOST_CHECK( std::string(ex.what()).find("-- \n") != std::string::npos );
#endif
            BOOST_CHECK( std::string(ex.what()).find("special exception") != std::string::npos );
            throw;
        }
    }
    catch (std::bad_cast &) {
        bad_cast = true;
    }
    BOOST_CHECK( bad_cast );
}

SENF_AUTO_TEST_CASE(errnoException)
{
    try {
        try {
            throw senf::SystemException("::open()", ENOENT) << "\nmore";
        }
        catch (senf::Exception & e) {
            e << "\nx=" << 1 << boost::format("\ny=%d") % 2;
            throw;
        }
    }
    catch (senf::SystemException & e) {
        BOOST_CHECK_EQUAL( e.errorNumber(), ENOENT );
        BOOST_CHECK_EQUAL( e.errorString(), "No such file or directory" );
        std::string what (e.what());
        std::string::size_type pos (what.find("-- \n"));
        if (pos != std::string::npos)
            what = std::string(what, pos+4);
        BOOST_CHECK_EQUAL( what, "[No such file or directory] ::open()\nmore\nx=1\ny=2" );
    }
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
