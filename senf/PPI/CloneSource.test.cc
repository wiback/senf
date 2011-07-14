// $Id$
//
// Copyright (C) 2007
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief CloneSource unit tests */

//#include "CloneSource.test.hh"
//#include "CloneSource.test.ih"

// Custom includes
#include "CloneSource.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include <senf/Packets/Packets.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(cloneSource)
{
    senf::PacketData::byte data[] = { 0xab };
    senf::Packet p (senf::DataPacket::create(data));

    senf::ppi::module::CloneSource source (p);
    senf::ppi::module::debug::ActiveSink sink;

    senf::ppi::connect(source,sink);
    senf::ppi::init();

    BOOST_CHECK( sink.request() != p );
    BOOST_CHECK( sink.request().data()[0] == p.data()[0] );
    BOOST_CHECK( sink.request().data()[0] == p.data()[0] );

    data[0] = 0xcd;
    senf::Packet p2 (senf::DataPacket::create(data));
    source.replacePacket( p2);
    BOOST_CHECK( sink.request().data()[0] == p2.data()[0] );
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
