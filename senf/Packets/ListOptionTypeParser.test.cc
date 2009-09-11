// $Id: ListOptionTypeParser.test.cc 1345 2009-08-26 15:40:55Z pug $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <Philipp.Batroff@fokus.fraunhofer.de>
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

// Custom includes
#include "Packets.hh"
#include <senf/Packets/DefaultBundle/IPv6ExtOptionType.hh>

#include <senf/Utils/hexdump.hh>
#include <senf/Utils/auto_unit_test.hh>

#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};

    struct OptionParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD( size, senf::UInt8Parser );
        typedef senf::detail::FixedAuxParserPolicy<senf::UInt8Parser, 1u> ListOptionTypeAuxPolicy;
        typedef senf::detail::ListOptionTypeParser_Policy<senf::GenericOptTypeTLVPacketParser, ListOptionTypeAuxPolicy> ListOptionTypePolicy;
        typedef senf::ListParser<ListOptionTypePolicy> ListOptionTypeParser;
        SENF_PARSER_FIELD ( list, ListOptionTypeParser);

        SENF_PARSER_FINALIZE(OptionParser);
    };
}

BOOST_AUTO_UNIT_TEST(ListOptionTypeParser)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            OptionParser::init_bytes));
    OptionParser p ( pi->data().begin() ,&pi->data());
    BOOST_CHECK_EQUAL(OptionParser::init_bytes +0, 7u);

    p.list().init();

    BOOST_CHECK_EQUAL( p.list().size(), 0u );
    BOOST_CHECK_EQUAL( p.list().bytes(), 6u ); //padding has to be accounted for!
    BOOST_CHECK( p.list().empty() );

     // the mutators are really tested together with the container wrappers since they are based
     // on the container wrapper. Here we only need one call to make the list larger ...

    // this doesn't make sense, since padding is added and the size doesn't really change
    // should be tested later with the container
    p.list().push_back_space();
    p = OptionParser(pi->data().begin(),&pi->data());
    BOOST_CHECK_EQUAL( p.list().bytes(), 6u );  //padding bytes
    BOOST_CHECK_EQUAL( p.list().size(), 1u );   //padding
    BOOST_CHECK_EQUAL( p.list().empty(), false );
}

BOOST_AUTO_UNIT_TEST(ListOptionTypeParser_container)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            OptionParser::init_bytes));

    {
        OptionParser p ( pi->data().begin() ,&pi->data());
        p.init();
        OptionParser::list_t::container c (p.list());

        BOOST_CHECK_EQUAL( c.size(), 0u );
        BOOST_CHECK_EQUAL( c.bytes(), 0u ); // padding bytes wont be in here, added/removed automatically in destructor
        BOOST_CHECK( c.begin() == c.end() );

        unsigned char d[]  = {0x65, 0x02, 0x40, 0x34};
        unsigned char d1[] = {0x03, 0x01, 0x77};
        unsigned char d2[] = {0x07, 0x01, 0x13};

        SENF_CHECK_NO_THROW( c.push_back( d ) );
        BOOST_CHECK_EQUAL( c.bytes(), 4u );
        BOOST_CHECK_EQUAL( c.size(), 1u );

        SENF_CHECK_NO_THROW( c.push_back( d1 ) );
        BOOST_CHECK_EQUAL( c.bytes(), 7u );
        BOOST_CHECK_EQUAL( c.size(), 2u );

        SENF_CHECK_NO_THROW( c.push_back( d2 ) );
        BOOST_CHECK_EQUAL( c.bytes(), 10u );
        BOOST_CHECK_EQUAL( c.size(), 3u );

        OptionParser::list_t::container::iterator cIter (c.begin());
        BOOST_CHECK_EQUAL( cIter->altAction(), 1u);
        BOOST_CHECK_EQUAL( cIter->changeFlag(), 1u);
        BOOST_CHECK_EQUAL( cIter->optionType(), 5u);
        BOOST_CHECK_EQUAL( cIter->optionLength(), 2u);
        BOOST_CHECK_EQUAL( *(boost::begin(cIter->value()) ), 0x40);
        BOOST_CHECK_EQUAL( *(boost::next(boost::begin(cIter->value()) )), 0x34);
        cIter++;
        BOOST_CHECK_EQUAL( cIter->optionType(), 3u);
        BOOST_CHECK_EQUAL( cIter->optionLength(), 1u);
        BOOST_CHECK_EQUAL( *(boost::begin(cIter->value() )), 0x77);
        cIter++;
        BOOST_CHECK_EQUAL( cIter->optionType(), 7u);
        BOOST_CHECK_EQUAL( cIter->optionLength(), 1u);
        BOOST_CHECK_EQUAL( *(boost::begin(cIter->value())), 0x13);

        //deletes first element
        c.erase(c.begin(),1);
        BOOST_CHECK_EQUAL( c.size(), 2u );
        BOOST_CHECK_EQUAL( c.bytes(), 6u );
        //deletes first 2 elements
        c.erase(c.begin(),1);
        BOOST_CHECK_EQUAL( c.size(), 1u );
        BOOST_CHECK_EQUAL( c.bytes(), 3u );
        BOOST_CHECK_EQUAL( c.empty(), false);

        //clear whole list
        SENF_CHECK_NO_THROW( c.clear() );
        BOOST_CHECK_EQUAL( c.size(), 0u );
        BOOST_CHECK_EQUAL( c.bytes(), 0u );
        BOOST_CHECK_EQUAL( c.empty(), true);
    }
}

// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
