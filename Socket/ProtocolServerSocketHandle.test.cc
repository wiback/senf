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

    struct MyProtocol : public satcom::lib::test::SomeProtocol
    {
        using satcom::lib::test::SomeProtocol::init_server;
        void init_server(char const *,unsigned) const {}
    };
}

BOOST_AUTO_UNIT_TEST(protocolServerSocketHandle)
{
    typedef satcom::lib::ProtocolServerSocketHandle<MyProtocol> MySocketHandle;

    {
        typedef satcom::lib::MakeSocketPolicy<
            satcom::lib::test::SomeFramingPolicy,
            satcom::lib::test::SomeReadPolicy,
            satcom::lib::test::SomeWritePolicy
            >::policy OtherSocketPolicy;
        typedef satcom::lib::SocketHandle<OtherSocketPolicy> OtherSocketHandle;

        MySocketHandle h;
        h.protocol();

        OtherSocketHandle osh (h);
        h = satcom::lib::static_socket_cast<MySocketHandle>(osh);
   
        MySocketHandle::ClientSocketHandle client = h.accept();
        BOOST_CHECK_EQUAL( client.fd(), -1 );

        BOOST_CHECK_EQUAL( h.dumpState(),
                           "handle: satcom::lib::ProtocolServerSocketHandle<(anonymous namespace)::MyProtocol>\n"
                           "file.handle: -1\n"
                           "file.refcount: 2\n"
                           "socket.policy: satcom::lib::SocketPolicy<satcom::lib::test::SomeAddressingPolicy, satcom::lib::test::SomeFramingPolicy, satcom::lib::test::SomeCommunicationPolicy, satcom::lib::test::SomeReadPolicy, satcom::lib::test::SomeWritePolicy, satcom::lib::test::SomeBufferingPolicy>\n"
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
// c-file-style: "satcom"
// End:
