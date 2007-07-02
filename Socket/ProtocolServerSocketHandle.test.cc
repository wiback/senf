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

//#include "ProtocolServerSocketHandle.test.hh"
//#include "ProtocolServerSocketHandle.test.ih"

// Custom includes
#include "ProtocolServerSocketHandle.hh"
#include "SocketProtocol.test.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct MyProtocol : public senf::test::SomeProtocol
    {
        using senf::test::SomeProtocol::init_server;
        void init_server(char const *,unsigned) const {}
    };
}

BOOST_AUTO_UNIT_TEST(protocolServerSocketHandle)
{
    typedef senf::ProtocolServerSocketHandle<MyProtocol> MySocketHandle;

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

        MySocketHandle::ClientSocketHandle client = h.accept();
        BOOST_CHECK_EQUAL( client.fd(), -1 );

        BOOST_CHECK_EQUAL( h.dumpState(),
                           "file.handle: -1\n"
                           "file.refcount: 2\n"
                           "handle: senf::ProtocolServerSocketHandle<(anonymous namespace)::MyProtocol>\n"
                           "socket.policy: senf::SocketPolicy<senf::test::SomeAddressingPolicy, senf::test::SomeFramingPolicy, senf::test::SomeCommunicationPolicy, senf::test::SomeReadPolicy, senf::test::SomeWritePolicy, senf::test::SomeBufferingPolicy>\n"
                           "socket.protocol: (anonymous namespace)::MyProtocol\n"
                           "socket.server: true\n" );

    }

    {
        MySocketHandle h("foo.bar.c",1234);
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
// End:
