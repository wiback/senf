// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief SyslogUDPTarget unit tests */

#include "SyslogUDPTarget.hh"

// Custom includes
#include <boost/format.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <senf/Socket/Protocols/INet/UDPSocketHandle.hh>
#include "Logger.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    int base_pid = 0;

    unsigned port(unsigned i)
    {
        if (! base_pid)
            base_pid = ::getpid();
        return 23456u + (((base_pid^(base_pid>>8)^(base_pid>>16)^(base_pid>>24))&0xff)<<2) + i;
    }

}

SENF_AUTO_TEST_CASE(syslogUDPTarget)
{
    using namespace boost::gregorian;

    senf::log::SyslogUDPTarget udplog (
        senf::INet4SocketAddress(senf::INet4Address::Loopback, port(0)));
    senf::UDPv4ClientSocketHandle server (
        senf::INet4SocketAddress(senf::INet4Address::Loopback, port(0)));

    udplog.tag("");
    udplog.showTime(false);
    udplog.showLevel(false);
    udplog.route();

    SENF_LOG(("Test message"));
    BOOST_CHECK_EQUAL( server.read(), "<13> Test message" );

    SENF_LOG(("Test message\nLine 2"));
    BOOST_CHECK_EQUAL( server.read(), "<13> Test message" );
    BOOST_CHECK_EQUAL( server.read(), "<13> Line 2" );

    udplog.timeFormat("%Y-%m-%d");
    udplog.showTime();
    SENF_LOG(("Very long message: " + std::string(896, 'x') ));

    date d (day_clock::local_day());
    BOOST_CHECK_EQUAL( server.read(), "<13> " + to_iso_extended_string(d) + " Very long message: " + std::string(896-5-10-19-1, 'x') );
    BOOST_CHECK_EQUAL( server.read(), "<13> " + to_iso_extended_string(d) + " " + std::string(5+10+19+1, 'x'));
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
