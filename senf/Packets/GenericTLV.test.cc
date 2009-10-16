// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief GenericTLV unit tests */

// Custom includes
#include "GenericTLV.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct MyTLVParserBase 
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type,   senf::UInt8Parser );
        SENF_PARSER_FIELD_RO ( length, senf::UInt8Parser );
        SENF_PARSER_FINALIZE ( MyTLVParserBase           );
    };
    
    struct MyGenericTLVParser
        : public senf::GenericTLVParserBase<MyTLVParserBase>
    {
        typedef senf::GenericTLVParserBase<MyTLVParserBase> base;
        MyGenericTLVParser(data_iterator i, state_type s) : base(i,s) {}
    };
        
   struct MyConcreteTLVParser
       : public MyTLVParserBase
   {
   #   include SENF_PARSER()
       SENF_PARSER_INHERIT  ( MyTLVParserBase             );
       SENF_PARSER_FIELD    ( myValue, senf::UInt32Parser );
       SENF_PARSER_FINALIZE ( MyConcreteTLVParser         );
    
       SENF_PARSER_INIT() {
           type() = typeId;
           length_() = 4;
       }
       static type_t::value_type const typeId = 0x42;
       
       void dump(std::ostream & os) const {
           boost::io::ios_all_saver ias(os);
           os << "  MyConcreteTLVParser\n"
              << "    type:   " << senf::format::dumpint(type()) << "\n"
              << "    length: " << senf::format::dumpint(length()) << "\n"
              << "    value:  " << senf::format::dumpint(myValue()) << "\n";
       }
   };
        
    class MyTestPacketParser
        : public senf::PacketParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_FIELD_RO ( list_length, senf::UInt8Parser );
        SENF_PARSER_LIST     ( tlv_list, list_length, MyGenericTLVParser );
        SENF_PARSER_FINALIZE ( MyTestPacketParser );
    };
        
    struct MyTestPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<MyTestPacketType>
    {
        typedef senf::PacketTypeMixin<MyTestPacketType> mixin;
        typedef MyTestPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    };
    typedef senf::ConcretePacket<MyTestPacketType> MyTestPacket;
}


BOOST_AUTO_UNIT_TEST(GenericTLV_parser)
{
    BOOST_CHECK_EQUAL( senf::init_bytes<MyGenericTLVParser>::value, 
            senf::init_bytes<MyTLVParserBase>::value) ;

    unsigned char data[] = {
            0x42, 0x04,             // type, length
            0x00, 0x01, 0x02, 0x03  // value
    };

    senf::DataPacket dataPacket ( senf::DataPacket::create(data));
    MyGenericTLVParser genericTLVParser (dataPacket.data().begin(), &dataPacket.data());

    BOOST_CHECK_EQUAL( senf::bytes( genericTLVParser), sizeof(data) );
    BOOST_CHECK_EQUAL( genericTLVParser.type(),   0x42 );
    BOOST_CHECK_EQUAL( genericTLVParser.length(), 0x04 );
    SENF_CHECK_EQUAL_COLLECTIONS( data+2, data+sizeof(data),
            genericTLVParser.value().begin(), genericTLVParser.value().end() );
    genericTLVParser.value( data );
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            genericTLVParser.value().begin(), genericTLVParser.value().end() );

    std::vector<unsigned char> value (4, 0xab);   
    genericTLVParser.value( std::make_pair(0x42, value));
    
    BOOST_CHECK( genericTLVParser.is<MyConcreteTLVParser>());

    MyConcreteTLVParser concreteTLVParser ( genericTLVParser.as<MyConcreteTLVParser>());
    BOOST_CHECK_EQUAL( concreteTLVParser.type(),   0x42 );
    BOOST_CHECK_EQUAL( concreteTLVParser.length(), 0x04 );
    BOOST_CHECK_EQUAL( concreteTLVParser.myValue(), 0xabababab );
}

BOOST_AUTO_UNIT_TEST(GenericTLV_packet)
{
    MyTestPacket p ( MyTestPacket::create());
    MyTestPacket::Parser::tlv_list_t::container tlvContainer (p->tlv_list() );
    MyConcreteTLVParser tlv ( tlvContainer.push_back_space().init<MyConcreteTLVParser>());
    tlv.myValue() << 0xffff;
    p.finalizeThis();
    
    unsigned char data[] = {
            0x01, // tlv list length
            // first tlv:
            0x42, 0x04,             // type, length
            0x00, 0x00, 0xff, 0xff  // value
    };
    SENF_CHECK_EQUAL_COLLECTIONS( data, data+sizeof(data),
            p.data().begin(), p.data().end() );
}


BOOST_AUTO_UNIT_TEST(GenericTLV_registry)
{
    MyTestPacket p ( MyTestPacket::create());
    MyTestPacket::Parser::tlv_list_t::container tlvContainer (p->tlv_list() );
    MyConcreteTLVParser conreteTLVParser ( 
            tlvContainer.push_back_space().init<MyConcreteTLVParser>());
    conreteTLVParser.myValue() << 0xffff;
    p.finalizeThis();
        
    std::stringstream ss;
    (*tlvContainer.begin()).dump( ss);
    BOOST_CHECK_EQUAL( ss.str().substr(0,58), 
            "  GenericTLVParser<(anonymous namespace)::MyTLVParserBase>" );
    
    senf::GenericTLVParserRegistry<MyTLVParserBase>::instance()
            .registerParser<MyConcreteTLVParser>();
    ss.str(""); ss.clear();
    
    (*tlvContainer.begin()).dump( ss);
    BOOST_CHECK_EQUAL( ss.str().substr(0,21), "  MyConcreteTLVParser" );
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
