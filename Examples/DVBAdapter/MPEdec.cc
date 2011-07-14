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

#include <string>
#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/dvb/dmx.h>

#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/MPEGDVBBundle/MPESection.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/hexdump.hh>
#include <senf/Socket/Protocols/DVB.hh>
#include <senf/Packets.hh>

#define PID 500

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

class MySniffer
{
    senf::DVBDemuxSectionHandle handle;

public:
    MySniffer(unsigned short adapter=0, unsigned short device=0)
        : handle( adapter, device )
    {
        struct dmx_sct_filter_params sec_filter;
        memset(&sec_filter, 0, sizeof (struct dmx_sct_filter_params));
        sec_filter.pid = PID;
        sec_filter.filter.filter[0] = 62;
        sec_filter.filter.mask[0] = 0xff;
        sec_filter.flags = DMX_IMMEDIATE_START;
        sec_filter.flags |= DMX_CHECK_CRC;

        handle.protocol().setSectionFilter( &sec_filter );

        senf::Scheduler::instance().add(
            handle, senf::membind(&MySniffer::dumpSection, this));
    }

private:
    void dumpSection(senf::Scheduler::EventId event)
    {
        std::string data (handle.read());
        senf::MPESection section (senf::MPESection::create(data));
        section.dump(std::cout);
        senf::PacketData & datagramData (section.next().data());
        senf::hexdump(datagramData.begin(), datagramData.end(), std::cout);
    }
};

int main(int argc, char const * argv[])
{
    try {
        MySniffer sniffer;
        senf::Scheduler::instance().process();
    }
    catch (std::exception const & ex) {
        std::cerr << senf::prettyName(typeid(ex)) << ": " << ex.what() << "\n";
    }
    return 0;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
