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
    \brief Exception.test unit tests */

//#include "Exception.test.hh"
//#include "Exception.test.ih"

// Custom includes
#include "Exception.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/format.hpp>
#include <errno.h>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(errnoException)
{
    BOOST_CHECK_THROW( senf::throwErrno(), senf::SystemException );
    BOOST_CHECK_THROW( senf::throwErrno(ENOENT), senf::SystemException );
    BOOST_CHECK_THROW( senf::throwErrno(""), senf::SystemException );
    BOOST_CHECK_THROW( senf::throwErrno("", ENOENT), senf::SystemException );

    try {
        try {
            senf::throwErrno("::open()", ENOENT);
        }
        catch(senf::SystemException & e) {
            e << ": x=" << 1 << boost::format(", y=%d") % 2;
            throw;
        }
    }
    catch (senf::SystemException & e) {
        BOOST_CHECK_EQUAL( e.errorNumber(), ENOENT );
        BOOST_CHECK_EQUAL( e.what(), "::open(): (2) No such file or directory: x=1, y=2" );
    }
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
