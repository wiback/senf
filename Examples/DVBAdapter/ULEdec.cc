// $Id$
//
// Copyright (C) 2007
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

// Definition of non-inline non-template functions

#include "ULEdec.hh"

#include <cassert>
#include <linux/dvb/dmx.h>
#include <boost/format.hpp>
#include <senf/Packets.hh>
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/membind.hh>

#define PID 271
#define TRANSPORT_PACKET_SIZE 188
// max. payload_pointer = ts packet payload size ( = ts packet size - ts header - payload_pointer)
//                          - 2 bytes min. sndu header
#define MAX_PAYLOAD_POINTER ( TRANSPORT_PACKET_SIZE - 4 - 1 - 2 )

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////


ULEdec::ULEdec(unsigned short adapter, unsigned short device)
    : demuxHandle( adapter, device ),  dvrHandle( adapter, device )
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

void ULEdec::handleEvent(senf::Scheduler::EventId event)
{
    senf::TransportPacket ts_packet (
            senf::TransportPacket::create(188, senf::noinit));
    dvrHandle.read( ts_packet.data() );
    
    // Check TS error conditions: sync_byte, transport_error_indicator, scrambling_control.
    if ( (ts_packet->sync_byte() != TRANSPORT_PACKET_SYNC_BYTE) ||
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
    iterator payload_iter = payloadData.begin();
    iterator payload_end = payloadData.end();

    std::cout << "New TS Packet:\n"
              << "----------------------------------------------------------------------------\n";
    senf::hexdump(payload_iter, payload_end, std::cout);
    std::cout << "----------------------------------------------------------------------------\n";

    // Synchronize continuity counter
    this->priv_tscc = ts_packet->continuity_counter();

    switch (ts_packet->pusi()) {
    case 0: {
        switch (this->receiver_state) {
        case Idle:
            return;
        case Reassembly:
            payload_iter = readContSNDUPacket( payload_iter, payload_end );
            if (isSDNUPacketComplete()) {
                this->receiver_state = Idle;
                try {
                    handleSNDUPacket();
                } catch (ULEdecException const & ex) {
                    std::cerr << ex.what() << "\n";
                    return;
                }
                switch (std::distance( payload_iter, payload_end ))
                case 0:
                case 1:
                    return;
                default:
                    if ( (*payload_iter++ << 8 | *payload_iter++) != ULE_END_INDICATOR )
                        std::cerr << "delimiting error\n";
            } else {
                assert( std::distance( payload_iter, payload_end ) == 0 );
            }
        }
        break;
    }
    case 1: {
        // a PUSI value of 1 indicates the presence of a Payload Pointer.
        unsigned char payload_pointer = *payload_iter++;
        if (payload_pointer > MAX_PAYLOAD_POINTER) {
            std::cerr << str( boost::format(
                    "invalid payload_pointer (%d)\n") % unsigned(payload_pointer) ) ;
            this->receiver_state = Idle;
            return;
        }
        switch (this->receiver_state) {
        case Idle:
            std::advance(payload_iter, payload_pointer);
            break;
        case Reassembly:
            // Reassembly Payload Pointer Checking
            if (snduPacketBytesLeft() != payload_pointer) {
                // delimiting error
                std::cerr << str( boost::format(
                        "delimiting error: bytes left in SNDU packet != payload_pointer ("
                        "(%d != %d)\n") % snduPacketBytesLeft() % payload_pointer );
                std::advance(payload_iter, payload_pointer);
            } else {
                payload_iter = readContSNDUPacket( payload_iter, payload_end );
                assert( isSDNUPacketComplete() );
                try {
                    handleSNDUPacket();
                } catch (ULEdecException const & ex) {
                    std::cerr << ex.what() << "\n";
                    this->receiver_state = Idle;
                    return;
                }
            }
        }
        this->receiver_state = Idle;
        try {
            do {
                payload_iter = readNewSNDUPacket( payload_iter, payload_end );
                if (! isSDNUPacketComplete()) {
                    assert( std::distance( payload_iter, payload_end ) == 0 );
                    this->receiver_state = Reassembly;
                    break;
                }
                handleSNDUPacket();
            } while (std::distance(payload_iter, payload_end) < 2 );
        }
        catch (ULEdecException const & ex) {
            std::cerr << ex.what() << "\n";
            return;
        }
    }

    } // end pusi-switch
}


ULEdec::iterator ULEdec::readContSNDUPacket(iterator i, iterator const i_end)
{
    if (priv_sndu_type_1) {
        this->snduPacket->type() |= *i++;
    }
    return readRawSNDUPacketData(i, i_end);
}


ULEdec::iterator ULEdec::readNewSNDUPacket(iterator i, iterator const i_end)
{
    bool dbit = false;
    senf::Packet::size_type sndu_length = *i++ << 8 | *i++;
    if (sndu_length & 0x8000) {
        sndu_length &= 0x7FFF;
        dbit = true;
    }
    if (sndu_length < 5 || sndu_length == 0xffff) {
        throw ULEdecException( str( boost::format(
                "SNDU length error. length=%d") % sndu_length) );
     }
    this->snduPacket = senf::SNDUPacket::create(sndu_length+4);

    if (dbit)
        this->snduPacket->withoutDestination();
    // else not needed since default on newly created packet is withDestination()

    this->snduPacket->length() = sndu_length;
    this->snduPacketData_iter = boost::next(this->snduPacket.data().begin(), 2);
    this->priv_sndu_type_1 = false;

    switch (std::distance(i, i_end)) {
    case 1:
        this->priv_sndu_type_1 = true;
        this->snduPacket->type() = *i++ << 8;
        this->snduPacketData_iter++;
    case 0:
        break;

    default:
        this->snduPacket->type() = *i++ << 8 | *i++;
        std::advance(this->snduPacketData_iter, 2);
        i = readRawSNDUPacketData(i, i_end);
    }

    return i;
}


ULEdec::iterator ULEdec::readRawSNDUPacketData(iterator i, iterator i_end)
{
    unsigned char how_much = std::min(
            snduPacketBytesLeft(), std::distance(i, i_end) );
    copy_n(i, how_much, this->snduPacketData_iter);
    std::advance(i, how_much);
    std::advance(this->snduPacketData_iter, how_much);
    return i;
}


void ULEdec::handleSNDUPacket()
{
    this->snduPacket.dump(std::cout);
    std::cout << "----------------------------------------------------------------------------\n\n";
    if (this->snduPacket->crc() != this->snduPacket->calcCrc()) {
        throw ULEdecException( str( boost::format(
                "CRC Error. received crc:%d calculated crc:%d")
                    % this->snduPacket->crc() % this->snduPacket->calcCrc() ) );
    }
//    senf::Packet nextPacket = this->snduPacket.next();
//    senf::hexdump(
//            nextPacket.data().begin(),
//            nextPacket.data().end(),
//            std::cout);

}


inline bool ULEdec::isSDNUPacketComplete()
{
    return this->snduPacketData_iter == this->snduPacket.data().end();
}

inline ULEdec::iterator::difference_type ULEdec::snduPacketBytesLeft()
{
    return std::distance( this->snduPacketData_iter, this->snduPacket.data().end() );
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
// compile-command: "scons -U"
// comment-column: 40
// End:
