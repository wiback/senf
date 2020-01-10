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


/** \file
    \brief Packet non-inline non-template implementation */

//#include "Packet.ih"

// Custom includes
#include "Packets.hh"

//#include "Packet.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SENF_DEBUG
std::int32_t senf::Packet::pktCount_ = 0;
#endif

prefix_ std::int32_t const & senf::Packet::pktCount()
{
#ifdef SENF_DEBUG
    return pktCount_;    
#else
    static std::int32_t noCount (-1);
    return noCount;
#endif
}

prefix_ senf::PacketInterpreterBase::ptr senf::Packet::getNext(PacketInterpreterBase::optional_range const & range)
    const
{
    factory_t factory (ptr()->nextPacketType());
    return parseNextAs(factory ? factory : DataPacket::factory(), range);
}

prefix_ senf::Packet senf::Packet::getLast()
    const
{
    Packet p (*this);
    Packet n (p.next(nothrow));
    while (n) {
        p = n;
        n = p.next(nothrow);
    }
    return p;
}

prefix_ void senf::Packet::dump(std::ostream & os, DumpPacketAnnotations_t dumpAnnotationsSwitch)
    const
{
    last(); // Make sure the packet is complete
    ptr()->dump(os, dumpAnnotationsSwitch);
}

prefix_ void senf::Packet::memDebug(std::ostream & os)
    const
{
    ptr()->impl().memDebug(os);
    os << "  handle @" << ptr().get() << std::endl;
    PacketInterpreterBase * p (first().ptr().get());
    p->memDebug(os);
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, Packet const & packet)
{
    os << "Packet<" << packet.typeId().prettyName() << ">:" << packet.id();
    return os;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
