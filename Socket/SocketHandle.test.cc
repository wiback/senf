// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Unit tests

//#include "SocketHandle.test.hh"
//#include "SocketHandle.test.ih"

// Custom includes
#include "SocketHandle.hh"
#include "SocketProtocol.test.hh"
#include "AddressingPolicy.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    namespace sl = senf;

    class MySocketHandle
        : public sl::SocketHandle<sl::test::SomeProtocol::Policy>
    {
    public:
        MySocketHandle()
            : sl::SocketHandle<sl::test::SomeProtocol::Policy>(
                std::auto_ptr<sl::SocketProtocol>(new sl::test::SomeProtocol()),false)
            {}
    };

    class FDHandle
        : public senf::FileHandle
    {
    public:
        FDHandle()
            : senf::FileHandle(std::auto_ptr<senf::FileBody>(
                                          new senf::FileBody())) {}
    };
}

BOOST_AUTO_UNIT_TEST(socketHandle)
{
    typedef sl::MakeSocketPolicy<
        sl::test::SomeCommunicationPolicy,
        sl::test::SomeReadPolicy
        >::policy OtherSocketPolicy;
    typedef sl::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

    MySocketHandle myh;
    OtherSocketHandle osh (myh);
    osh = myh;

    typedef sl::SocketHandle<sl::test::SomeProtocol::Policy> SomeSocketHandle;
    SomeSocketHandle ssh = senf::static_socket_cast<SomeSocketHandle>(osh);

    BOOST_CHECK_NO_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(osh) );

    typedef sl::SocketHandle< sl::MakeSocketPolicy<
        OtherSocketPolicy,
        senf::NoAddressingPolicy
        >::policy> SomeOtherSocketHandle;

    BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeOtherSocketHandle>(osh),
                       std::bad_cast );
    BOOST_CHECK_THROW( senf::dynamic_socket_cast<SomeSocketHandle>(
                           senf::FileHandle(FDHandle())),
                       std::bad_cast );

    BOOST_CHECK_EQUAL( myh.dumpState(),
                       "file.handle: -1\n"
                       "file.refcount: 3\n"
                       "handle: senf::SocketHandle<senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy, senf::test::SomeBufferingPolicy> >\n"
                       "socket.policy: senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy, senf::test::SomeBufferingPolicy>\n"
                       "socket.protocol: senf::test::SomeProtocol\n"
                       "socket.server: false\n" );
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
