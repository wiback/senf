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
    \brief ServerSocketHandle unit tests */

#include "ServerSocketHandle.hh"

// Custom includes
#include "ClientSocketHandle.hh"
#include "AddressingPolicy.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include "SocketProtocol.test.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::ServerSocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::ServerSocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::auto_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(true)))
            {}
    };

}

SENF_AUTO_TEST_CASE(serverSocketHandle)
{
    typedef senf::MakeSocketPolicy<
        senf::test::SomeFramingPolicy,
        senf::test::SomeReadPolicy,
        senf::test::SomeWritePolicy
        >::policy OtherSocketPolicy;
    typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

    MySocketHandle myh;
    OtherSocketHandle osh (myh);
    osh = myh;

    typedef senf::ServerSocketHandle<senf::test::SomeSocketProtocol::Policy> SomeSocketHandle;
    SomeSocketHandle ssh = senf::static_socket_cast<SomeSocketHandle>(osh);

    typedef senf::ServerSocketHandle<senf::MakeSocketPolicy<
        OtherSocketPolicy,
        senf::NoAddressingPolicy
        >::policy> SomeOtherSocketHandle;
    typedef senf::ClientSocketHandle<OtherSocketPolicy> OtherClientHandle;

    SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );
    BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                       std::bad_cast );
    BOOST_CHECK_THROW( senf::dynamic_socket_cast<OtherClientHandle>(osh),
                       std::bad_cast );

    SENF_CHECK_NO_THROW( myh.bind(0) );
    BOOST_CHECK_EQUAL( myh.local(), 2u );

    {
        MySocketHandle::ClientHandle client = myh.accept();
        BOOST_CHECK_EQUAL( client.fd(), -1 );
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
