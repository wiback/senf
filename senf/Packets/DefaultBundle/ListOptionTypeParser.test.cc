// $Id$
//
// Copyright (C) 2009
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
//   Philipp Batroff <pug@berlios.de>

// Custom includes
#include <senf/Packets/Packets.hh>
#include "IPv6ExtOptions.hh"
#include "ListOptionTypeParser.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase
    {};

    struct OptionParser : public senf::PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD( size, senf::UInt8Parser );
        typedef senf::detail::FixedAuxParserPolicy<senf::UInt8Parser, 1u> ListOptionTypeAuxPolicy;
        typedef senf::detail::ListOptionTypeParser_Policy<
            senf::IPv6GenericOptionParser, ListOptionTypeAuxPolicy> ListOptionTypePolicy;
        typedef senf::ListParser<ListOptionTypePolicy> ListOptionTypeParser;
        SENF_PARSER_FIELD ( list, ListOptionTypeParser);

        SENF_PARSER_FINALIZE(OptionParser);
    };
}

SENF_AUTO_UNIT_TEST(ListOptionTypeParser)
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

SENF_AUTO_UNIT_TEST(ListOptionTypeParser_container)
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

        std::vector<unsigned char> d (2, 0xab);
        std::vector<unsigned char> d1 (1, 0x77);
        std::vector<unsigned char> d2 (1, 0x13);

        SENF_CHECK_NO_THROW( c.push_back( std::make_pair(0x65, d) ));
        BOOST_CHECK_EQUAL( c.bytes(), 4u );
        BOOST_CHECK_EQUAL( c.size(), 1u );

        SENF_CHECK_NO_THROW( c.push_back( std::make_pair(0x03, d1) ));
        BOOST_CHECK_EQUAL( c.bytes(), 7u );
        BOOST_CHECK_EQUAL( c.size(), 2u );

        SENF_CHECK_NO_THROW( c.push_back( std::make_pair(0x07, d2) ));
        BOOST_CHECK_EQUAL( c.bytes(), 10u );
        BOOST_CHECK_EQUAL( c.size(), 3u );

        OptionParser::list_t::container::iterator cIter (c.begin());
        BOOST_CHECK_EQUAL( cIter->altAction(), 1u);
        BOOST_CHECK_EQUAL( cIter->changeFlag(), 1u);
        BOOST_CHECK_EQUAL( cIter->optionType(), 5u);
        BOOST_CHECK_EQUAL( cIter->length(), 2u);
        BOOST_CHECK_EQUAL( *(boost::begin(cIter->value()) ), 0xab);
        BOOST_CHECK_EQUAL( *(boost::next(boost::begin(cIter->value()) )), 0xab);
        cIter++;
        BOOST_CHECK_EQUAL( cIter->optionType(), 3u);
        BOOST_CHECK_EQUAL( cIter->length(), 1u);
        BOOST_CHECK_EQUAL( *(boost::begin(cIter->value() )), 0x77);
        cIter++;
        BOOST_CHECK_EQUAL( cIter->optionType(), 7u);
        BOOST_CHECK_EQUAL( cIter->length(), 1u);
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
//-/////////////////////////////////////////////////////////////////////////////////////////////////


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
