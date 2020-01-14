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
