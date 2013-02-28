// $Id$
//
// Copyright (C) 2007
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Exception unit tests */

//#include "Exception.test.hh"
//#include "Exception.test.ih"

// Custom includes
#include "Exception.hh"
#include <boost/format.hpp>
#include <errno.h>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(wrapException)
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

SENF_AUTO_UNIT_TEST(errnoException)
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
