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
    \brief ServerSocketHandle unit tests */

//#include "ServerSocketHandle.test.hh"
//#include "ServerSocketHandle.test.ih"

// Custom includes
#include "ServerSocketHandle.hh"
#include "ClientSocketHandle.hh"
#include "SocketProtocol.test.hh"
#include "AddressingPolicy.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

SENF_AUTO_UNIT_TEST(serverSocketHandle)
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
