// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
#include "Socket/DVBDemuxHandles.hh"
#include "Packets/ParseInt.hh"
#include "Packets/Packet.hh"
#include "Packets/PacketData.hh"

#define PID 271

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    static const unsigned BLOCK_SIZE = 16;

    template <class Iterator>
    void hexdump(Iterator i, Iterator const & i_end, std::ostream& stream)
    {
        unsigned offset (0);
        std::string ascii;
        for (; i != i_end; ++i, ++offset) {
            switch (offset % BLOCK_SIZE) {
            case 0:
                if (!ascii.empty()) {
                    stream << "  " << ascii << "\n";
                    ascii = "";
                }
                stream << "  "
                          << std::hex << std::setw(4) << std::setfill('0')
                          << offset << ' ';
                break;
            case BLOCK_SIZE/2:
                stream << " ";
                ascii += ' ';
                break;
            }
            stream << ' ' << std::hex << std::setw(2) << std::setfill('0')
                      << unsigned(*i);
            ascii += (*i >= ' ' && *i < 126) ? *i : '.';
        }
        if (!ascii.empty()) {
            for (; (offset % BLOCK_SIZE) != 0; ++offset) {
                if ((offset % BLOCK_SIZE) == BLOCK_SIZE/2)
                    stream << " ";
                stream << "   ";
            }
            stream << "  " << ascii << "\n";
        }
        stream << std::dec;
    }
}


class MySniffer
{
    senf::DVBDemuxPESHandle demuxHandle;
    senf::DVBDvrHandle dvrHandle;

public:
    MySniffer()
    {
        struct dmx_pes_filter_params pes_filter;
        memset(&pes_filter, 0, sizeof (struct dmx_pes_filter_params));
        pes_filter.pid = PID;
        pes_filter.input  = DMX_IN_FRONTEND;
        pes_filter.output = DMX_OUT_TS_TAP;
        pes_filter.pes_type = DMX_PES_OTHER;
        pes_filter.flags = DMX_IMMEDIATE_START;

        demuxHandle.protocol().setPESFilter( &pes_filter );
        
        senf::Scheduler::instance().add(
            dvrHandle, senf::membind(&MySniffer::dumpSection, this));
    }

private:
    void dumpSection(senf::FileHandle /* ignored */, senf::Scheduler::EventId event)
    {
        std::string data (dvrHandle.read());
        std::cout << data.length() << "\n";
        //senf::DatagramSection section (senf::DatagramSection::create(data));
        //section.dump(std::cout);
        //senf::PacketData & datagramData (section.last().data());
        //hexdump(datagramData.begin(), datagramData.end(), std::cout);
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
