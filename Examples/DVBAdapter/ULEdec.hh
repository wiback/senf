//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
