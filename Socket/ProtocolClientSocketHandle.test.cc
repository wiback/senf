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

//#include "ProtocolClientSocketHandle.test.hh"
//#include "ProtocolClientSocketHandle.test.ih"

// Custom includes
#include "ProtocolClientSocketHandle.hh"
#include "SocketProtocol.test.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct MyProtocol : public senf::test::SomeProtocol
    {
        using senf::test::SomeProtocol::init_client;
        void init_client(char const *,unsigned) const {}
    };
}

BOOST_AUTO_UNIT_TEST(protocolClientSocketHandle)
{
    typedef senf::ProtocolClientSocketHandle<MyProtocol> MySocketHandle;

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
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
