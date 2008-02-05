// $Id: ULEdec.cc 355 2007-07-26 14:17:02Z tho $
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

#include <senf/Scheduler/Scheduler.hh>
#include <senf/Packets/MPEGDVBBundle/TransportPacket.hh>
#include <senf/Packets/MPEGDVBBundle/SNDUPacket.hh>
#include <senf/Socket/Protocols/DVB.hh>


class ULEdec
{
public:
    ULEdec(unsigned short adapter=0, unsigned short device=0);

private:
    typedef senf::PacketData::iterator iterator;
    
    enum ReceiverState {
        Idle,
        Reassembly
    };
    
    senf::DVBDemuxPESHandle demuxHandle;
    senf::DVBDvrHandle dvrHandle;
    
    senf::SNDUPacket snduPacket;
    
    ReceiverState receiver_state;
    unsigned char priv_tscc;
    bool priv_sndu_type_1;
    iterator snduPacketData_iter;

    void handleEvent(senf::Scheduler::EventId event);
    void handleTSPacket(senf::TransportPacket tsPacket);
    void handleSNDUPacket();
    
    iterator readNewSNDUPacket(iterator i, const iterator i_end);
    iterator readContSNDUPacket(iterator i, const iterator i_end);
    iterator readRawSNDUPacketData(iterator i, iterator const i_end);
    
    inline bool isSDNUPacketComplete(); 
    inline iterator::difference_type snduPacketBytesLeft();
    
};


struct ULEdecException : public std::exception
{
    ULEdecException(char const * what) : what_(what) {};
    ULEdecException(std::string const what) : what_(what) {};
    virtual char const * what() const throw() { return what_.c_str(); }
    virtual ~ULEdecException() throw() {};
    std::string what_;
};



// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
