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

//#include "PacketRegistry.test.hh"
//#include "PacketRegistry.test.ih"

// Custom includes
#include <string>
#include "PacketRegistry.hh"
#include "DataPacket.hh"
#include "ParseInt.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    using namespace senf;

    struct BaseTag {
        typedef unsigned key_t;
    };

    struct StringTag {
        typedef std::string key_t;
    };

    class BasePacket
        : public Packet, public PacketRegistryMixin<BaseTag,BasePacket>
    {
        using PacketRegistryMixin<BaseTag,BasePacket>::registerInterpreter;
    public:
        typedef ptr_t<BasePacket>::ptr ptr;
        typedef iterator byte_iterator;

        typedef Parse_UInt16<iterator> Parse_Type;

        Parse_Type type() const { return Parse_Type(begin()); }
        static bool check(iterator b, iterator e) { return true; }

    private:
        template <class Arg>
        BasePacket(Arg const & arg) : Packet(arg) {}

        virtual void v_nextInterpreter() const
            { registerInterpreter(type(), begin()+2, end()); }
        virtual void v_finalize() {}
        virtual void v_dump(std::ostream & os) const {}

        friend class Packet;
    };

    class FooPacket : public Packet
    {
    public:
        typedef ptr_t<FooPacket>::ptr ptr;
        typedef iterator byte_iterator;

        static bool check(iterator b, iterator e) { return true; }

    private:
        template <class Arg>
        FooPacket(Arg const & arg) : Packet(arg) {}

        virtual void v_nextInterpreter() const {}
        virtual void v_finalize() {}
        virtual void v_dump(std::ostream & os) const {}

        friend class Packet;
    };

    class BarPacket : public Packet
    {
    public:
        typedef ptr_t<BarPacket>::ptr ptr;
        typedef iterator byte_iterator;

        static bool check(iterator b, iterator e) { return true; }

    private:
        template <class Arg>
        BarPacket(Arg const & arg) : Packet(arg) {}

        virtual void v_nextInterpreter() const {}
        virtual void v_finalize() {}
        virtual void v_dump(std::ostream & os) const {}

        friend class Packet;
    };

    namespace reg {
        PacketRegistry<StringTag>::RegistrationProxy<FooPacket> registerFoo ("foo");
        PacketRegistry<StringTag>::RegistrationProxy<BarPacket> registerBar ("bar");
    }

}

BOOST_AUTO_UNIT_TEST(packetRegistry_test)
{
    unsigned char data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

    {
        BasePacket::ptr p (Packet::create<BasePacket>(data, data+sizeof(data)));
        BOOST_CHECK( p->next()->is<DataPacket>() );
    }

    PacketRegistry<BaseTag>::registerPacket<FooPacket>(1u);
    PacketRegistry<BaseTag>::registerPacket<BarPacket>(2u);

    BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<FooPacket>(), 1u );
    BOOST_CHECK_EQUAL( PacketRegistry<BaseTag>::key<BarPacket>(), 2u );
    BOOST_CHECK_THROW( PacketRegistry<BaseTag>::key<DataPacket>(), PacketTypeNotRegistered );

    {
        BasePacket::ptr p (Packet::create<BasePacket>(data, data+sizeof(data)));
        BOOST_CHECK( p->next()->is<FooPacket>() );
    }

    data[1] = 0x02;

    {
        BasePacket::ptr p (Packet::create<BasePacket>(data, data+sizeof(data)));
        BOOST_CHECK( p->next()->is<BarPacket>() );
    }

    data[0] = 0x01;

    {
        BasePacket::ptr p (Packet::create<BasePacket>(data, data+sizeof(data)));
        BOOST_CHECK( p->next()->is<DataPacket>() );
    }

    BOOST_CHECK_EQUAL( PacketRegistry<StringTag>::key<FooPacket>(), "foo" );
    BOOST_CHECK( PacketRegistry<StringTag>::create("foo",data,data+sizeof(data))
                 ->is<FooPacket>() );

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
