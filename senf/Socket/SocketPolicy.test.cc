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
    \brief SocketPolicy unit tests */

#include "SocketPolicy.hh"

// Custom includes
#include <boost/mpl/assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/utility.hpp> // enable_if

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "SocketPolicy.test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_AUTO_TEST_CASE(socketPolicy)
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
