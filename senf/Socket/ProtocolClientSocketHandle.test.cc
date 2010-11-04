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
    \brief ProtocolClientSocketHandle unit tests */

//#include "ProtocolClientSocketHandle.test.hh"
//#include "ProtocolClientSocketHandle.test.ih"

// Custom includes
#include "ProtocolClientSocketHandle.hh"
#include "SocketProtocol.test.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct MySocketProtocol : public senf::test::SomeSocketProtocol
    {
        using senf::test::SomeSocketProtocol::init_client;
        void init_client(char const *,unsigned) const {}
    };
}

SENF_AUTO_UNIT_TEST(protocolClientSocketHandle)
{
    typedef senf::ProtocolClientSocketHandle<MySocketProtocol> MySocketHandle;

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
    }

    {
        MySocketHandle hh("foo.bar.c",1234);
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
