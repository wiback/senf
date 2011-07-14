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
    \brief ProtocolServerSocketHandle unit tests */

//#include "ProtocolServerSocketHandle.test.hh"
//#include "ProtocolServerSocketHandle.test.ih"

// Custom includes
#include "ProtocolServerSocketHandle.hh"
#include "SocketProtocol.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct MySocketProtocol : public senf::test::SomeSocketProtocol
    {
        using senf::test::SomeSocketProtocol::init_server;
        void init_server(char const *,unsigned) const {}
    };
}

SENF_AUTO_UNIT_TEST(protocolServerSocketHandle)
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
