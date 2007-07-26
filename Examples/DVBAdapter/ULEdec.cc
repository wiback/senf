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
#include "Packets/MPEGDVBBundle/TransportPacket.hh"
#include "Packets/MPEGDVBBundle/SNDUPacket.hh"
#include "Utils/membind.hh"
#include "Socket/Protocols/DVB/DVBDemuxHandles.hh"
#include "Packets/ParseInt.hh"
#include "Packets/Packet.hh"
#include "Packets/PacketData.hh"
#include "Packets/ParseInt.hh"

#define PID 271
#define TS_SYNC 0x47

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


class ULEdec
{
    senf::DVBDemuxPESHandle demuxHandle;
    senf::DVBDvrHandle dvrHandle;
    
    unsigned char receiver_state;
    unsigned char priv_tscc;
public:
    ULEdec()
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
            dvrHandle, senf::membind(&ULEdec::handlePacket, this));
        
        receiver_state = 1; // Idle
    }

private:
    void handlePacket(senf::FileHandle, senf::Scheduler::EventId event)
    {
        char ts_data[188];
        dvrHandle.read(&ts_data[0], &ts_data[188]);
        senf::TransportPacket tsPacket (
                senf::TransportPacket::create( boost::begin(ts_data) ));
//        packet.dump(std::cout);
//        senf::PacketData & packetData (packet.last().data());
//        hexdump(packetData.begin(), packetData.end(), std::cout);
        
        // Check TS error conditions: sync_byte, transport_error_indicator, scrambling_control.
        if ( (tsPacket->sync_byte() != TS_SYNC) || 
             (tsPacket->transport_error_indicator() == true) || 
             (tsPacket->transport_scrmbl_ctrl() != 0)) 
        {
            std::cerr << "invalid ts packet\n";
            // drop partly decoded SNDU, reset state, resync on PUSI.
            return;
        }

        unsigned char payload_pointer;
        switch (receiver_state) {
        case 1:  // Idle State
            // resync on PUSI
            if (tsPacket->pusi() == 0) 
                return; // skip packet
            // Synchronize continuity counter
            priv_tscc = tsPacket->continuity_counter();
            // a PUSI value of 1 indicates the presence of a Payload Pointer.
            payload_pointer = ts_data[4];
            if (payload_pointer>181) {
                std::cerr << "invalid payload_pointer\n";
                return;
            }
            
            // 
            break;
        case 2:  // Reassembly State
            break;
        }
    }
};

int main(int argc, char const * argv[])
{
    try {
        ULEdec decoder;
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
