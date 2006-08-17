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

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    namespace sl = satcom::lib;
    
    struct INetAddressingPolicy : public sl::AddressingPolicyBase {};
    struct UnixAddressingPolicy : public sl::AddressingPolicyBase {};

    struct StreamFramingPolicy : public sl::FramingPolicyBase {};
    struct DgramFramingPolicy : public sl::FramingPolicyBase {};

    struct ConnectedCommunicationPolicy : public sl::CommunicationPolicyBase {};
    struct UnconnectedCommunicationPolicy : public sl::CommunicationPolicyBase {};

    struct ReadablePolicy : public sl::ReadPolicyBase {};
    struct UnreadablePolicy : public sl::ReadPolicyBase {};

    struct WritablePolicy : public sl::WritePolicyBase {};
    struct UnwritablePolicy : public sl::WritePolicyBase {};
    
    struct SocketBufferingPolicy : public sl::BufferingPolicyBase {};

    typedef sl::MakeSocketPolicy<
        INetAddressingPolicy,
        StreamFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WritablePolicy,
        SocketBufferingPolicy
        >::policy MyProtocol_Policy;

    class MyProtocol 
        : public sl::ConcreteSocketProtocol<MyProtocol_Policy>
    {
    public:
        ~MyProtocol() {}
    };

}

BOOST_AUTO_UNIT_TEST(protocolServerSocketHandle)
{
    typedef sl::ProtocolServerSocketHandle<MyProtocol> MySocketHandle;

    MySocketHandle h;
    h.protocol();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
