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
    \brief Format.test unit tests */

#include "Format.hh"

// Custom includes
#include <sstream>
#include <iomanip>
#include <boost/cstdint.hpp>
#include "String.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(formatEng)
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

SENF_AUTO_TEST_CASE(dumpint)
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

namespace {
    void f1(std::ostream & os) {
        senf::format::IndentHelper indent;
        os << indent << "f1\n";
    }
    void f2(std::ostream & os) {
        senf::format::IndentHelper indent;
        os << indent << "f2_1\n";
        f1( os);
        os << indent << "f2_2\n";
        indent.increase();
        os << indent << "f2_3\n";
    }
}
SENF_AUTO_TEST_CASE(indent)
{
    std::stringstream ss;
    f2(ss);
    BOOST_CHECK_EQUAL( ss.str(),
            "  f2_1\n"
            "    f1\n"
            "  f2_2\n"
            "    f2_3\n");
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
