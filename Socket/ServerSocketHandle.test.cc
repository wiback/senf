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

//#include "ServerSocketHandle.test.hh"
//#include "ServerSocketHandle.test.ih"

// Custom includes
#include "ServerSocketHandle.hh"
#include "SocketProtocol.test.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    namespace sl = satcom::lib;

    class MySocketHandle
        : public sl::ServerSocketHandle<sl::test::SomeProtocol::Policy>
    {
    public:
        MySocketHandle()
            : sl::ServerSocketHandle<sl::test::SomeProtocol::Policy>(std::auto_ptr<sl::SocketProtocol>(new sl::test::SomeProtocol()))
            {}
    };
}

BOOST_AUTO_UNIT_TEST(serverSocketHandle)
{
    typedef sl::MakeSocketPolicy<
        sl::test::SomeFramingPolicy,
        sl::test::SomeReadPolicy,
        sl::test::SomeWritePolicy
        >::policy OtherSocketPolicy;
    typedef sl::SocketHandle<OtherSocketPolicy> OtherSocketHandle;
    
    MySocketHandle myh;
    OtherSocketHandle ssh (myh);
    ssh = myh;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
