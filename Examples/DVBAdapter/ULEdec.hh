// $Id: ULEdec.cc 355 2007-07-26 14:17:02Z tho $
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


class ULEdec
{
public:
    ULEdec();

private:
    typedef senf::PacketData::iterator iterator;
    
    enum ReciverState {
        Idle,
        Reassembly
    };
    
    senf::DVBDemuxPESHandle demuxHandle;
    senf::DVBDvrHandle dvrHandle;
    
    senf::SNDUPacket snduPacket;
    
    ReciverState receiver_state;
    unsigned char priv_tscc;
    bool priv_sndu_type_1;
    iterator snduPacketData_iter;

    void handleEvent(senf::FileHandle, senf::Scheduler::EventId event);
    void handleTSPacket(senf::TransportPacket tsPacket);
    void handleSNDUPacket();
    
    iterator readNewSNDUPacket(iterator i_start, iterator i_end);
    iterator readContSNDUPacket(iterator i_start, iterator i_end);
    iterator readRawSNDUPacketData(iterator i_start, iterator i_end);
    
    inline bool isSDNUPacketComplete(); 
    inline iterator::difference_type snduPacketBytesLeft();
    
};


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
