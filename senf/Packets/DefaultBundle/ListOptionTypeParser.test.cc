//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include <senf/Packets/Packets.hh>
#include "ListOptionTypeParser.hh"

// Custom includes
#include "IPv6ExtOptions.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(ListOptionTypeParser)
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

SENF_AUTO_TEST_CASE(ListOptionTypeParser_container)
{
    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            OptionParser::init_bytes));

    {
        OptionParser p ( pi->data().begin() ,&pi->data());
        p.init();
        OptionParser::list_t::container_type c (p.list());

        BOOST_CHECK_EQUAL( c.size(), 0u );
        BOOST_CHECK_EQUAL( c.bytes(), 0u ); // padding bytes won't be in here, added/removed automatically in destructor
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

        OptionParser::list_t::container_type::iterator cIter (c.begin());
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
