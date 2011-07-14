// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief ClientSocketHandle unit tests */

//#include "ClientSocketHandle.test.hh"
//#include "ClientSocketHandle.test.ih"

// Custom includes
#include "SocketPolicy.test.hh"
#include "SocketProtocol.test.hh"
#include "ClientSocketHandle.hh"
#include "AddressingPolicy.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::ClientSocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::auto_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(false)))
            {}
    };

}

SENF_AUTO_UNIT_TEST(clientSocketHandle)
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
