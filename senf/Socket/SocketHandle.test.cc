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
    \brief SocketHandle unit tests */

#include "SocketHandle.hh"

// Custom includes
#include "AddressingPolicy.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "SocketProtocol.test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::SocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::SocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::unique_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(false, 0)))
            {}
    };

    class FDHandle
        : public senf::FileHandle
    {
    public:
        FDHandle() : senf::FileHandle(std::unique_ptr<senf::FileBody>(new senf::FileBody())) {}
    };

}

SENF_AUTO_TEST_CASE(socketHandle)
{
    typedef senf::MakeSocketPolicy<
        senf::test::SomeCommunicationPolicy,
        senf::test::SomeReadPolicy
        >::policy OtherSocketPolicy;
    typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

    typedef senf::MakeSocketPolicy<
        senf::test::SomeCommunicationPolicy,
        senf::test::SomeAddressingPolicy
        >::policy AnotherSocketPolicy;
    typedef senf::SocketHandle<AnotherSocketPolicy> AnotherSocketHandle;

    {
        MySocketHandle myh;
        OtherSocketHandle osh (myh);
        osh = myh;

        typedef senf::SocketHandle<senf::test::SomeSocketProtocol::Policy> SomeSocketHandle;
        SomeSocketHandle ssh = senf::static_socket_cast<SomeSocketHandle>(osh);

        SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );
        SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<AnotherSocketHandle>(osh) );

        typedef senf::SocketHandle< senf::MakeSocketPolicy<
            OtherSocketPolicy,
            senf::NoAddressingPolicy
            >::policy> SomeOtherSocketHandle;

        BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                           std::bad_cast );
        BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(
                               senf::FileHandle(FDHandle())),
                           std::bad_cast );

        BOOST_CHECK_EQUAL( myh.dumpState(),
                "file.handle: 0\n"
                "file.refcount: 3\n"
                "handle: senf::SocketHandle<senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy> >\n"
                "socket.protocol: senf::test::SomeSocketProtocol\n"
                "socket.protocol.policy: senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy>\n"
                "socket.server: false\n"
                "valid: true\n" );

        SENF_CHECK_NO_THROW( myh.facet<senf::test::SomeSocketProtocol>() );
    }

    // Ensure, the destructor is called and calls the correct close() implementation
    BOOST_CHECK( senf::test::SomeSocketProtocol::closeCount() >= 1u );
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
