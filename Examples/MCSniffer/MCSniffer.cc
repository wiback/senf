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
//   Thorsten Horstmann <tho@berlios.de>


// Definition of non-inline non-template functions

//#include "MCSniffer.hh"
//#include "MCSniffer.ih"

// Custom includes
#include <fstream>
#include <string>
#include <iomanip>
#include <senf/Socket/Protocols/INet.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>


//#include "MCSniffer.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

class MCSniffer
{
    senf::UDPv4ClientSocketHandle sock;
    std::ostream& stream;
    senf::scheduler::FdEvent event;

public:
    MCSniffer(senf::INet4Address addr, std::ostream& s)
        : stream(s), event("MCSniffer", senf::membind(&MCSniffer::dumpPacket, this),
                           sock, senf::scheduler::FdEvent::EV_READ)
    {
        // sock.bind(addr);
        sock.protocol().mcLoop(true);
        sock.protocol().mcAddMembership(addr);
    }

private:
    void dumpPacket(int event)
    {
        std::string data (sock.read());
        senf::EthernetPacket packet (
            senf::EthernetPacket::create(data));
        packet.dump(stream);
        senf::hexdump(packet.last().data().begin(),
                packet.last().data().end(),
                stream);
        stream << "\n\n";
     }
};


int main(int argc, char const * argv[])
{
    try {
        std::ofstream f1 ("233.132.152.1.txt");
        std::ofstream f2 ("233.132.152.2.txt");

        MCSniffer sniffer1 (
            senf::INet4Address::from_string("233.132.152.1"), f1);
        MCSniffer sniffer2 (
            senf::INet4Address::from_string("233.132.152.2"), f2);

        senf::scheduler::process();
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MCSniffer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
