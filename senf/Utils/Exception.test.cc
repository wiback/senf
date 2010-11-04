// $Id$
//
// Copyright (C) 2007
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
        catch(senf::Exception & e) {
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
