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
#include <algorithm>
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

#include "ULEdec.hh"

#define PID 271
#define TS_SYNC 0x47

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

template <class Iterator>
void ULEdec::hexdump(Iterator i, Iterator const & i_end, std::ostream& stream)
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


ULEdec::ULEdec()
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
        dvrHandle, senf::membind(&ULEdec::handleEvent, this));
    
    this->receiver_state = Idle;
    this->priv_sndu_type_1 = false;
}

void ULEdec::handleEvent(senf::FileHandle, senf::Scheduler::EventId event)
{
    senf::TransportPacket ts_packet (
            senf::TransportPacket::create(188, senf::TransportPacket::noinit));
    dvrHandle.read( ts_packet.data() );
   
    // Check TS error conditions: sync_byte, transport_error_indicator, scrambling_control.
    if ( (ts_packet->sync_byte() != TS_SYNC) || 
         (ts_packet->transport_error_indicator() == true) || 
         (ts_packet->transport_scrmbl_ctrl() != 0)) 
    {
        std::cerr << "invalid ts packet\n";
        // drop partly decoded SNDU, reset state, resync on PUSI.
        return;
    }
    
    handleTSPacket(ts_packet);
}
    
void ULEdec::handleTSPacket(senf::TransportPacket ts_packet)
{
    senf::PacketData & payloadData (ts_packet.next().data());
    iterator payload_start = payloadData.begin();
    iterator payload_end = payloadData.end();
    
    hexdump(payload_start, payload_end, std::cout);
    
    // Synchronize continuity counter
    this->priv_tscc = ts_packet->continuity_counter();
    
    if (ts_packet->pusi() == 0) {
        switch (this->receiver_state) {
        case Idle:
            return;
        case Reassembly:
            readContSNDUPacket( payload_start, payload_end );
        }
    } else {
        // a PUSI value of 1 indicates the presence of a Payload Pointer.
        unsigned char payload_pointer = *payload_start++;
        if (payload_pointer>181) {
            std::cerr << "invalid payload_pointer\n";
            return;
        }
        switch (this->receiver_state) {
        case Idle:
            payload_start += payload_pointer;
            readNewSNDUPacket( payload_start, payload_end );
            this->snduPacket.dump(std::cout);
            break;
        case Reassembly:
            readContSNDUPacket( payload_start, payload_end );
        }       
    }

}


ULEdec::iterator ULEdec::readContSNDUPacket(iterator i_start, iterator i_end)
{
    if (priv_sndu_type_1) {
        this->snduPacket->type() |= *i_start++;
    }
    i_start = readRawSNDUPacketData(i_start, i_end);
    
    return i_start;
}


ULEdec::iterator ULEdec::readNewSNDUPacket(iterator i_start, iterator i_end)
{ 
    bool dbit = false;
    senf::Packet::size_type sndu_length;
    sndu_length = *i_start++ << 8 | *i_start++;
    if (sndu_length & 0x8000) {
        sndu_length &= 0x7FFF;
        dbit = true;
    }
    this->snduPacket = senf::SNDUPacket::create(sndu_length+4);
    this->snduPacket->d_bit() = dbit;
    this->snduPacket->length() = sndu_length;
    this->snduPacketData_iter = this->snduPacket.data().begin() + 2;
    
    switch (std::distance(i_start, i_end)) {
    case 1:
        this->priv_sndu_type_1 = true;;
        this->snduPacket->type() = *i_start++;
        this->snduPacketData_iter++;
    case 0:
        break;
        
    default: 
        this->snduPacket->type() = *i_start++ | *i_start++;
        this->snduPacketData_iter += 2;
        i_start = readRawSNDUPacketData(i_start, i_end);
    }
    
    return i_start;
}


ULEdec::iterator ULEdec::readRawSNDUPacketData(iterator i_start, iterator i_end)
{
    unsigned char how_much = std::min(
            std::distance( this->snduPacketData_iter, this->snduPacket.data().end() ),
            std::distance( i_start, i_end ) );
    copy_n(i_start, how_much, this->snduPacketData_iter);
    i_start += how_much;
    this->snduPacketData_iter += how_much;
    return i_start;
}


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
