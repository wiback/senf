// $Id$
//
// Copyright (C) 2009 
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
    \brief Format.test unit tests */

//#include "Format.test.hh"
//#include "Format.test.ih"

// Custom includes
#include <sstream>
#include <iomanip>
#include "Format.hh"
#include <boost/cstdint.hpp>
#include "String.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_UNIT_TEST(formatEng)
{
    std::stringstream ss;

#   define CheckFormat(v,s)                                             \
        {                                                               \
            ss.str("");                                                 \
            ss << senf::format::eng v;                                  \
            BOOST_CHECK_EQUAL( ss.str(), s );                           \
        }

    ss << std::setw(21);

    CheckFormat( (0.0),                      "            0.000    " );
    CheckFormat( (-1.4431288e5, 2.26338e4),  "-144.313+-022.634e+03" );
    CheckFormat( (1.4444e-13, 2.111111e-16), " 144.440+-000.211e-15" );
    CheckFormat( (1.345e-3, 3.456),          "   0.001+-003.456    " );

    ss << std::setprecision(4) << std::uppercase << std::showpoint;

    CheckFormat( (0.0),                      "              0.0E+00" );
    CheckFormat( (-1.4431288e5, 2.26338e4),  "    -144.3+-022.6E+03" );
    CheckFormat( (1.4444e-13, 2.111111e-16), "     144.4+-000.2E-15" );
    CheckFormat( (1.345e-3, 3.456),          "       0.0+-003.5E+00" );

    ss << std::showbase << std::internal << std::showpos << std::setfill('0');

    CheckFormat( (0.0),                      "+00000000000000000.0 " );
    CheckFormat( (-1.4431288e5, 2.26338e4),  "-0000000144.3+-022.6k" );
    CheckFormat( (1.4444e-13, 2.111111e-16), "+0000000144.4+-000.2f" );
    CheckFormat( (1.345e-3, 3.456),          "+0000000000.0+-003.5 " );

    ss << std::left << std::noshowpos << std::setfill('*');

    CheckFormat( (0.0),                      "   0.0 **************" );
    CheckFormat( (-1.4431288e5, 2.26338e4),  "-144.3+-022.6k*******" );
    CheckFormat( (1.4444e-13, 2.111111e-16), " 144.4+-000.2f*******" );
    CheckFormat( (1.345e-3, 3.456),          "   0.0+-003.5 *******" );

    ss << std::setw(0);

    CheckFormat( (0.0),                      "0.0" );
    CheckFormat( (-1.4431288e5, 2.26338e4),  "-144.3+-22.6k" );
    CheckFormat( (1.4444e-13, 2.111111e-16), "144.4+-0.2f" );
    CheckFormat( (1.345e-3, 3.456),          "0.0+-3.5" );

#   undef CheckFormat
    
    // From documentation
    
    {
        std::stringstream ss;
        ss << senf::format::eng(1.23);
        BOOST_CHECK_EQUAL(ss.str(), "1.230");
    }

    {
        std::stringstream ss;
        ss << std::setw(1) << senf::format::eng(1.23);
        BOOST_CHECK_EQUAL( ss.str(), "   1.230    " );
    }

    {
        std::stringstream ss;
        ss << std::setw(25) << std::setprecision(5) << std::showpos << std::uppercase 
           << std::internal << senf::format::eng(12345,67);
        BOOST_CHECK_EQUAL( ss.str(), "+       12.35+-000.07E+03" );
    }

    {
        std::stringstream ss;
        ss << std::showbase << senf::format::eng(12345,67);
        BOOST_CHECK_EQUAL( ss.str(), "12.345+-0.067k" );
    }

    // class member formatting
    BOOST_CHECK_EQUAL( senf::str(senf::format::eng(12345, 67)
                                 .setw()
                                 .setprecision(5)
                                 .setfill('0')
                                 .showbase()
                                 .showpos()
                                 .internal()),
                       "+012.35+-000.07k" );

    BOOST_CHECK_EQUAL( senf::str(senf::format::eng(12.345, 67)
                                 .setw()
                                 .setprecision(5)
                                 .showpoint()
                                 .uppercase()),
                       "  12.35+-067.00E+00" );
}

SENF_AUTO_UNIT_TEST(dumpint)
{
    std::stringstream ss;
    
#   define CheckFormat(v,s)                                             \
        {                                                               \
            ss.str("");                                                 \
            ss << senf::format::dumpint(v);                             \
            BOOST_CHECK_EQUAL( ss.str(), s );                           \
        }

    CheckFormat( boost::int8_t(32), " 0x20 (  32) ( )" );
    CheckFormat( boost::uint16_t(1234), "0x04d2 ( 1234) (..)" );

#   undef CheckFormat
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
