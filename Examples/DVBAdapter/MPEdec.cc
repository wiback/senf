// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

#include <string>
#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/dvb/dmx.h> 

#include "Scheduler/Scheduler.hh"
#include "Packets/DefaultBundle/EthernetPacket.hh"
#include "Packets/MPEGDVBBundle/DatagramSection.hh"
#include "Utils/membind.hh"
#include "Utils/hexdump.hh"
#include "Socket/Protocols/DVB/DVBDemuxHandles.hh"
#include "Packets/ParseInt.hh"
#include "Packets/Packet.hh"
#include "Packets/PacketData.hh"

#define PID 500

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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
    void dumpSection(senf::FileHandle /* ignored */, senf::Scheduler::EventId event)
    {
        std::string data (handle.read());
        senf::DatagramSection section (senf::DatagramSection::create(data));
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


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
