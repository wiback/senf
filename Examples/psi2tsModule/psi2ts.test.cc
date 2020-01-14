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


/** \file
    \brief psi2ts unit tests */

// Custom includes
#include <senf/PPI/DebugModules.hh>
#include <senf/PPI/Setup.hh>
#include <senf/Packets/MPEGDVBBundle/MPESection.hh>
#include <senf/Packets/MPEGDVBBundle/TransportPacket.hh>
#include "psi2ts.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>
#include <boost/lambda/lambda.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

void check_transportpacket_header(senf::TransportPacket tsPacket, bool pusi, unsigned pid, unsigned counter)
{
    BOOST_CHECK_EQUAL( tsPacket->sync_byte(),                 senf::TransportPacketType::SYNC_BYTE+0 );
    BOOST_CHECK_EQUAL( tsPacket->transport_error_indicator(), false                                  );
    BOOST_CHECK_EQUAL( tsPacket->pusi(),                      pusi                                   );
    BOOST_CHECK_EQUAL( tsPacket->transport_priority(),        false                                  );
    BOOST_CHECK_EQUAL( tsPacket->transport_scrmbl_ctrl(),     0x0u                                   );
    BOOST_CHECK_EQUAL( tsPacket->adaptation_field_ctrl(),     0x1u                                   );
    BOOST_CHECK_EQUAL( tsPacket->continuity_counter(),        counter                                );
    if (pusi)
        BOOST_CHECK_EQUAL( tsPacket->pointer_field(),         0x0u                                   );

}

template<class InputIterator, class T>
bool equal_elements(InputIterator first, InputIterator last, const T& value)
{
    return std::find_if( first, last, boost::lambda::_1 != value) == last;
}


SENF_AUTO_TEST_CASE(one_section_to_one_transportpacket)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;
    unsigned PID = 42;
    Psi2TsModule psi2ts (PID);

    senf::ppi::connect( source, psi2ts);
    senf::ppi::connect( psi2ts, sink);
    senf::ppi::init();

    std::string sec_data ( "psi2ts_test: one_section_to_one_transportpacket");
    senf::Packet sec_packet (senf::DataPacket::create(sec_data));
    sec_packet.finalize();

    source.submit(sec_packet);
    BOOST_CHECK_EQUAL( sink.size(), 1u);

    senf::TransportPacket ts_packet = sink.pop_front().as<senf::TransportPacket>();
    check_transportpacket_header( ts_packet, true, PID, 1);
    senf::PacketData & ts_payload_data = ts_packet.next().data();
    BOOST_CHECK_EQUAL_COLLECTIONS(
            ts_payload_data.begin(),
            boost::next( ts_payload_data.begin(), sec_data.size()),
            sec_data.begin(),
            sec_data.end());
    BOOST_CHECK( equal_elements(
            boost::next( ts_payload_data.begin(), ts_payload_data.size()),
            ts_payload_data.end(),
            0xffu));
}

SENF_AUTO_TEST_CASE(one_section_to_two_transportpackets)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;
    unsigned PID = 42;
    Psi2TsModule psi2ts (PID);

    senf::ppi::connect( source, psi2ts);
    senf::ppi::connect( psi2ts, sink);
    senf::ppi::init();

    std::string sec_data ( 183, 0x42);
    std::string sec_data2 ( "psi2ts_test: one_section_to_two_transportpackets");
    sec_data.append( sec_data2);
    senf::Packet sec_packet (senf::DataPacket::create(sec_data));
    sec_packet.finalize();

    source.submit( sec_packet);
    BOOST_CHECK_EQUAL( sink.size(), 2u);

    senf::TransportPacket ts_packet = sink.pop_front().as<senf::TransportPacket>();
    check_transportpacket_header( ts_packet, true, PID, 1);
    senf::PacketData & ts_payload_data1 = ts_packet.next().data();

    BOOST_CHECK( equal_elements( ts_payload_data1.begin(), ts_payload_data1.end(), 0x42));

    ts_packet = sink.pop_front().as<senf::TransportPacket>();
    check_transportpacket_header( ts_packet, false, PID, 2);
    senf::PacketData & ts_payload_data2 = ts_packet.next().data();
    BOOST_CHECK_EQUAL_COLLECTIONS(
            ts_payload_data2.begin(),
            boost::next( ts_payload_data2.begin(), sec_data2.size()),
            sec_data2.begin(),
            sec_data2.end());
    BOOST_CHECK( equal_elements(
            boost::next( ts_payload_data2.begin(), sec_data2.size()),
            ts_payload_data2.end(),
            0xffu));
}

SENF_AUTO_TEST_CASE(many_sections_to_many_transportpackets)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;
    unsigned PID = 42;
    Psi2TsModule psi2ts (PID);

    senf::ppi::connect( source, psi2ts);
    senf::ppi::connect( psi2ts, sink);
    senf::ppi::init();

    std::string sec_data ( "many_sections_to_many_transportpackets");
    senf::Packet sec_packet (senf::DataPacket::create(sec_data));
    sec_packet.finalize();

    unsigned NUMBER_OF_SECTIONS = 42u;
    for (unsigned i=1; i<=NUMBER_OF_SECTIONS; i++) {
        source.submit( sec_packet);
    }
    BOOST_CHECK_EQUAL( sink.size(), NUMBER_OF_SECTIONS);

    for (unsigned i=1; i<=NUMBER_OF_SECTIONS; i++) {
        senf::TransportPacket ts_packet = sink.pop_front().as<senf::TransportPacket>();
        check_transportpacket_header( ts_packet, true, PID, i%16);
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
