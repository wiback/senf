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
    \brief ProtocolClientSocketHandle unit tests */

#include "ProtocolClientSocketHandle.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "SocketProtocol.test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct MySocketProtocol : public senf::test::SomeSocketProtocol
    {
        using senf::test::SomeSocketProtocol::init_client;
        void init_client(char const *,unsigned) const {}
    };
}

SENF_AUTO_TEST_CASE(protocolClientSocketHandle)
{
    typedef senf::ProtocolClientSocketHandle<MySocketProtocol> MySocketHandle;

    {
        typedef senf::MakeSocketPolicy<
            senf::test::SomeFramingPolicy,
            senf::test::SomeReadPolicy,
            senf::test::SomeWritePolicy
            >::policy OtherSocketPolicy;
        typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

        MySocketHandle h;
        h.protocol();

        OtherSocketHandle osh (h);
        h = senf::static_socket_cast<MySocketHandle>(osh);
    }

    {
        MySocketHandle hh("foo.bar.c",1234);
    }
    BOOST_CHECK( true );
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
