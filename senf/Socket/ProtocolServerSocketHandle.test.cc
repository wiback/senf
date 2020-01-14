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
    \brief ProtocolServerSocketHandle unit tests */

#include "ProtocolServerSocketHandle.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "SocketProtocol.test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct MySocketProtocol : public senf::test::SomeSocketProtocol
    {
        using senf::test::SomeSocketProtocol::init_server;
        void init_server(char const *,unsigned) const {}
    };
}

SENF_AUTO_TEST_CASE(protocolServerSocketHandle)
{
    typedef senf::ProtocolServerSocketHandle<MySocketProtocol> MySocketHandle;

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

        MySocketHandle::ClientHandle client = h.accept();
        BOOST_CHECK_EQUAL( client.fd(), -1 );

        BOOST_CHECK_EQUAL( h.dumpState(),
                "handle: senf::ProtocolServerSocketHandle<(anonymous namespace)::MySocketProtocol>\n"
                "valid: false\n" );
    }

    {
        MySocketHandle h("foo.bar.c",1234);
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
