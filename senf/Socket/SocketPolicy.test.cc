// $Id$
//
// Copyright (C) 2006
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
    \brief SocketPolicy unit tests */

#include "SocketPolicy.test.hh"
//#include "SocketPolicy.test.ih"

// Custom includes
#include <boost/mpl/assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/utility.hpp> // enable_if

#include "SocketPolicy.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

using namespace senf;

namespace {
    struct INetAddressingPolicy : public AddressingPolicyBase {};
    struct UnixAddressingPolicy : public AddressingPolicyBase {};

    struct StreamFramingPolicy : public FramingPolicyBase {};
    struct DgramFramingPolicy : public FramingPolicyBase {};

    struct ConnectedCommunicationPolicy : public CommunicationPolicyBase {};
    struct UnconnectedCommunicationPolicy : public CommunicationPolicyBase {};

    struct ReadablePolicy : public ReadPolicyBase {};
    struct UnreadablePolicy : public ReadPolicyBase {};

    struct WritablePolicy : public WritePolicyBase {};
    struct UnwritablePolicy : public WritePolicyBase {};

    template <class SPolicy>
    struct ConvertibleValue
    {
        ConvertibleValue() {}
        ConvertibleValue(ConvertibleValue const & other) {}

        template <class OtherPolicy>
        ConvertibleValue(ConvertibleValue<OtherPolicy> const & other,
                         typename boost::enable_if< SocketPolicyIsBaseOf<SPolicy,OtherPolicy> >::type * = 0)
            {}

        ConvertibleValue const & operator=(ConvertibleValue const & other)
            { return *this; }

        template <class OtherPolicy>
        typename boost::enable_if< SocketPolicyIsBaseOf<SPolicy,OtherPolicy>,
                                   ConvertibleValue >::type const &
        operator=(ConvertibleValue<OtherPolicy> const & other)
            { return *this; }
    };

}

SENF_AUTO_UNIT_TEST(socketPolicy)
{
    // Most of these checks are really compile-time checks ...

    typedef MakeSocketPolicy<
        UnixAddressingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy>::policy Policy1;

    typedef SocketPolicy<
        UnixAddressingPolicy,
        UnspecifiedFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        UnspecifiedWritePolicy> Policy2;

    BOOST_MPL_ASSERT(( boost::is_same<Policy1,Policy2> ));

    typedef MakeSocketPolicy<
        Policy1,
        UnspecifiedCommunicationPolicy>::policy Policy3;

    typedef SocketPolicy<
        UnixAddressingPolicy,
        UnspecifiedFramingPolicy,
        UnspecifiedCommunicationPolicy,
        ReadablePolicy,
        UnspecifiedWritePolicy> Policy4;

    BOOST_MPL_ASSERT(( boost::is_same<Policy3,Policy4> ));
    BOOST_MPL_ASSERT_NOT(( boost::is_same<Policy1, Policy3> ));

    BOOST_MPL_ASSERT(( SocketPolicyIsBaseOf<Policy3,Policy1> ));
    BOOST_MPL_ASSERT_NOT(( SocketPolicyIsBaseOf<Policy1,Policy3> ));
    BOOST_MPL_ASSERT_NOT(( SocketPolicyIsBaseOf<Policy1,int> ));

    // The following should fail at compile time
    // BOOST_MPL_ASSERT(( SocketPolicyIsBaseOf<Policy1,Policy3> ));

    {
        ConvertibleValue<Policy1> p1;
        ConvertibleValue<Policy3> p3(p1);

        p3 = p1;
        // The following should fail at compile time
        // p1 = p3;
    }
    
    {
        Policy1 p1;
        Policy3 p3;

        BOOST_CHECK_THROW( Policy1::checkBaseOf(p3), std::bad_cast );
        SENF_CHECK_NO_THROW( Policy3::checkBaseOf(p1) );
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
