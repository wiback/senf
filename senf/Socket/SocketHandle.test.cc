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
    \brief SocketHandle unit tests */

//#include "SocketHandle.test.hh"
//#include "SocketHandle.test.ih"

// Custom includes
#include "SocketHandle.hh"
#include "SocketProtocol.test.hh"
#include "AddressingPolicy.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    class MySocketHandle
        : public senf::SocketHandle<senf::test::SomeSocketProtocol::Policy>
    {
    public:
        MySocketHandle()
            : senf::SocketHandle<senf::test::SomeSocketProtocol::Policy>(
                std::auto_ptr<senf::SocketBody>(
                    new senf::ProtocolSocketBody<senf::test::SomeSocketProtocol>(false, 0)))
            {}
    };

    class FDHandle
        : public senf::FileHandle
    {
    public:
        FDHandle() : senf::FileHandle(std::auto_ptr<senf::FileBody>(new senf::FileBody())) {}
    };

}

BOOST_AUTO_UNIT_TEST(socketHandle)
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
