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
    \brief ClientSocketHandle unit tests */

#include "ClientSocketHandle.hh"

// Custom includes
#include "SocketPolicy.test.hh"
#include "SocketProtocol.test.hh"
#include "AddressingPolicy.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::unique_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(false)))
            {}
    };

}

SENF_AUTO_TEST_CASE(clientSocketHandle)
{
    BOOST_CHECKPOINT("Constructing socket handle");
    MySocketHandle myh;

    // conversion to other socket handles
    {
        typedef senf::MakeSocketPolicy<
            senf::test::SomeFramingPolicy,
            senf::test::SomeReadPolicy,
            senf::test::SomeWritePolicy
            >::policy OtherSocketPolicy;
        typedef senf::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

        BOOST_CHECKPOINT("Copy-constructing socket handle");
        OtherSocketHandle osh (myh);
        BOOST_CHECKPOINT("Assigning socket handle");
        osh = myh;
        typedef senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy> SomeSocketHandle;
        BOOST_CHECKPOINT("static_casting socket handle");
        SomeSocketHandle ssh =
            senf::static_socket_cast<SomeSocketHandle>(osh);
        SENF_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );
        typedef senf::ClientSocketHandle<senf::MakeSocketPolicy<
            OtherSocketPolicy,
            senf::NoAddressingPolicy
            >::policy> SomeOtherSocketHandle;
        BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                           std::bad_cast );
    }

    // reading and writing
    SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.read(), "TEST-READ" ) );
    {
        std::string buf("FOO-BAR");
        SENF_CHECK_NO_THROW( myh.read(buf,0) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ" );
    }
    {
        char buf[11];
        ::strcpy(buf,"0123456789");
        SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.read(buf,buf+10), buf+9 ) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ9" );
    }

    SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.readfrom().first, "TEST-READ" ) );
    {
        std::string buf("FOO-BAR");
        unsigned addr;
        SENF_CHECK_NO_THROW( myh.readfrom(buf,addr,0) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ" );
    }
    {
        char buf[11];
        unsigned addr;
        ::strcpy(buf,"0123456789");
        SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.readfrom(buf,buf+10,addr), buf+9 ) );
        BOOST_CHECK_EQUAL( buf, "TEST-READ9" );
    }

    {
        std::string s ("TEST-WRITE");
        SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.write(s)-s.begin(), 10 ) );
        s = "TEST";
        // This simulates a closed file in this test policy. However, we
        // have changed the semantics so this will not work anymore.
        // BOOST_CHECK_THROW( myh.write(s),senf::SystemException );
        char const * const s1 = "TEST-WRITE9";
        SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.write(s1,s1+10), s1+10u ) );
        s = "TEST-WRITE";
        SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.writeto(0,s)-s.begin(), 10 ) );
        SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.writeto(0,s1,s1+10), s1+10 ) );
    }

    SENF_CHECK_NO_THROW( myh.connect(0) );
    SENF_CHECK_NO_THROW( myh.bind(0) );
    SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.peer(), 1u ) );
    SENF_CHECK_NO_THROW( BOOST_CHECK_EQUAL( myh.local(), 2u ) );
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
