//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief MMapSocketProtocol non-inline non-template implementation */

#include "MMapSocketProtocol.hh"
//#include "MMapSocketProtocol.ih"

// Custom includes
#include <linux/if_packet.h>
#include <linux/net_tstamp.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include "MMapSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PACKET_TX_HAS_OFF
#define PACKET_TX_HAS_OFF 19
#endif
#ifndef PACKET_QDISC_BYPASS
#define PACKET_QDISC_BYPASS	20
#endif

prefix_ void senf::MMapSocketProtocol::close()
{
    close_mmap();
    SocketProtocol::close();
}

prefix_ void senf::MMapSocketProtocol::terminate()
    const
{
    terminate_mmap();
    SocketProtocol::terminate();
}

prefix_ void senf::MMapSocketProtocol::init_mmap(unsigned frameSize, unsigned rxqlen,
                                                 unsigned txqlen, unsigned reserve, bool qDiscBypass)
    const
{
    ::memset(&qi_, 0, sizeof(qi_));
    qi_.frameSize = frameSize;

    int v = TPACKET_V2;
    ::socklen_t l = sizeof(int);
    if (getsockopt(fd(), SOL_PACKET, PACKET_HDRLEN, (char *)&v, &l) != 0)
        SENF_THROW_SYSTEM_EXCEPTION("::getsockopt(SOL_PACKET, PACKET_HDRLEN)");
    qi_.hdrlen = TPACKET_ALIGN(v);

    v = TPACKET_V2;
    if (setsockopt(fd(), SOL_PACKET, PACKET_VERSION, (char *)&v, sizeof(v)) != 0 )
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_VERSION)");

    unsigned size (0);

    struct ::tpacket_req req;
    ::memset(&req, 0, sizeof(req));

    if (txqlen > 0) {
        v = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_LOSS, (char *)&v, sizeof(v)) != 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_LOSS");
    }

    if (rxqlen > 0) {
        qi_.reserveSize = 0;
        if (reserve > 0
            && setsockopt(fd(), SOL_PACKET, PACKET_RESERVE, (char *)&reserve, sizeof(reserve)) == 0)
            qi_.reserveSize = reserve;
        req.tp_frame_nr = rxqlen;
        req.tp_frame_size = frameSize;
        req.tp_block_size = req.tp_frame_nr * req.tp_frame_size;
        req.tp_block_nr = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_RX_RING,
                       reinterpret_cast<char *>(&req), sizeof(req)) != 0 )
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_RX_RING");
        size += req.tp_block_size;
    }

    if (txqlen > 0) {
        req.tp_frame_nr = txqlen;
        req.tp_frame_size = frameSize;
        req.tp_block_size = req.tp_frame_nr * req.tp_frame_size;
        req.tp_block_nr = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_TX_RING,
                       reinterpret_cast<char *>(&req), sizeof(req)) != 0 )
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_TX_RING");
#ifdef SENF_ENABLE_TPACKET_OFFSET
        v = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_TX_HAS_OFF, (char *)&v, sizeof(v)) != 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_TX_HAS_OFF");
#endif
        size += req.tp_block_size;
    }

    unsigned char * map (static_cast<unsigned char *>(
                    ::mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd(), 0)));
    if (map == MAP_FAILED)
        SENF_THROW_SYSTEM_EXCEPTION("::mmap()");

    qi_.map = map;
    qi_.init(rxqlen, txqlen);
    
    // supposedly this should speed up TX. And it does (<5% increase), when our non-blocking patch is applied. 
    // Otherwise it seems to rather costs us 10% 
    if (qDiscBypass) {
        v = 1;
        setsockopt(fd(), SOL_PACKET, PACKET_QDISC_BYPASS, (char*)&v, sizeof(v));
    }
    
    senf::FileHandleAccess::extraPtr(fh(), &qi_);
}

prefix_ void senf::MMapSocketProtocol::close_mmap()
    const
{
    if (! qi_.map)
        return;
    if (::munmap(qi_.map, (qi_.rx.end - qi_.rx.begin) + (qi_.tx.end - qi_.tx.begin)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::munmap");
    ::memset(&qi_, 0, sizeof(qi_));
}

prefix_ void senf::MMapSocketProtocol::terminate_mmap()
    const
{
    if (! qi_.map)
        return;
    ::munmap(qi_.map, (qi_.rx.end - qi_.rx.begin) + (qi_.tx.end - qi_.tx.begin));
    ::memset(&qi_, 0, sizeof(qi_));
}

prefix_ senf::detail::QueueInfo::TxStats senf::MMapSocketProtocol::txStats()
    const
{
    detail::QueueInfo::TxStats txStats (qi_.txStats);
    ::memset(&qi_.txStats, 0, sizeof(qi_.txStats));
    return txStats;
}

prefix_ senf::detail::QueueInfo::RxStats senf::MMapSocketProtocol::rxStats()
    const
{
    detail::QueueInfo::RxStats rxStats (qi_.rxStats);
    ::memset(&qi_.rxStats, 0, sizeof(qi_.rxStats));
    return rxStats;
}

prefix_ unsigned senf::MMapReadableSocketProtocol::available()
    const
{
    // Since dequeue() will skip outgoing packets it's impossible to return a correct
    // value here without race conditions. Thus we always return the frame size
    return static_cast<detail::QueueInfo *>(senf::FileHandleAccess::extraPtr(fh()))->frameSize;
}

prefix_ bool senf::MMapSocketProtocol::interfaceDead()
    const
{
    bool rtn (qi_.interfaceDead());
    ::memset(&qi_.txStats, 0, sizeof(qi_.txStats));
    ::memset(&qi_.rxStats, 0, sizeof(qi_.rxStats));
    return rtn;
}

prefix_ void senf::MMapSocketProtocol::timestamping(int sofFlags)
{
    if (setsockopt(fd(), SOL_PACKET, PACKET_TIMESTAMP, (char*)&sofFlags, sizeof(sofFlags)) != 0)
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_TIMESTAMP");   
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MMapSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
