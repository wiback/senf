// $Id$
//
// Copyright (C) 2008
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
    \brief psi2ts unit tests */

// Custom includes
#include "senf/PPI/DebugModules.hh"
#include "senf/PPI/Setup.hh"
#include <senf/Packets/MPEGDVBBundle/MPESection.hh>
#include <senf/Packets/MPEGDVBBundle/TransportPacket.hh>
#include "psi2ts.hh"

#include "../../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/lambda/lambda.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

void check_transportpacket_header(senf::TransportPacket tsPacket, bool pusi, unsigned pid, unsigned counter)
{
    BOOST_CHECK_EQUAL( tsPacket->sync_byte(),                 senf::TransportPacketType::SYNC_BYTE+0 );
    BOOST_CHECK_EQUAL( tsPacket->transport_error_indicator(), false                                  );
    BOOST_CHECK_EQUAL( tsPacket->pusi(),                      pusi                                   );
    BOOST_CHECK_EQUAL( tsPacket->transport_priority(),        false                                  );
    BOOST_CHECK_EQUAL( tsPacket->transport_scrmbl_ctrl(),     0x0u                                   );
    BOOST_CHECK_EQUAL( tsPacket->adaptation_field_ctrl(),     0x1u                                   );
    BOOST_CHECK_EQUAL( tsPacket->continuity_counter(),        counter                                );
        
}

BOOST_AUTO_UNIT_TEST(one_section_to_one_transportpacket)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;
    Psi2TsModule psi2ts;
    
    senf::ppi::connect(source, psi2ts);
    senf::ppi::connect(psi2ts, sink);
    senf::ppi::init();
    
    std::string payload_data ( "psi2ts_test: one_section_to_one_transportpacket");
    senf::Packet payload (senf::DataPacket::create(payload_data));
    payload.finalize();
    
    source.submit(payload);
    BOOST_CHECK_EQUAL( sink.size(), 1u);

    senf::TransportPacket tsPacket = sink.pop_front().as<senf::TransportPacket>();
    check_transportpacket_header( tsPacket, true, 0, 1);
    senf::PacketData & ts_data = tsPacket.next().data();
    BOOST_CHECK_EQUAL_COLLECTIONS( 
            ts_data.begin(), 
            boost::next( ts_data.begin(), payload_data.size()),
            payload_data.begin(),
            payload_data.end());
//    BOOST_CHECK( std::find_if( 
//            boost::next( ts_data.begin(), payload_data.size()), 
//            ts_data.end(),
//            boost::lambda::_1 != 0xffu) == ts_data.end() );

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
