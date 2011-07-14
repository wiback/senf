// $Id: ULEdec.cc 355 2007-07-26 14:17:02Z tho $
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


struct ULEdecException : public senf::Exception
{
    ULEdecException(std::string const & what)
        : senf::Exception(what) {}
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
