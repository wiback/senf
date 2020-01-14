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
    \brief TransportPacket non-inline non-template implementation */

#include "TransportPacket.hh"
//#include "TransportPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::TransportPacket );

prefix_ void senf::TransportPacketParser::init_fields()
    const
{
    sync_byte() = TransportPacketType::SYNC_BYTE;
    transport_error_indicator() = 0;
    transport_scrmbl_ctrl() = 0;
    adaptation_field_ctrl() = 0x1u;
}

prefix_ senf::UInt8Parser senf::TransportPacketParser::pointer_field()
    const
{
    return pointer_field_().get<1>();
}

prefix_ void senf::TransportPacketParser::setPUSI(bool pusi)
    const
{
    if (pusi) pointer_field_().init<1>();
    else      pointer_field_().init<0>();
}

prefix_ void senf::TransportPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "TransportPacket:\n"
       << std::hex
       << senf::fieldName("syncByte")                  << "0x" << unsigned(p->sync_byte()) << "\n"
       << senf::fieldName("transport error ind.")      << "0x" << unsigned(p->transport_error_indicator()) << "\n"
       << senf::fieldName("payload unit start ind.")   << "0x" << unsigned(p->pusi()) << "\n"
       << senf::fieldName("transport priority")        << "0x" << unsigned(p->transport_priority()) << "\n"
       << std::dec
       << senf::fieldName("pid")                       << unsigned(p->pid()) << "\n"
       << std::hex
       << senf::fieldName("transport scrambling c.")   << "0x" << unsigned(p->transport_scrmbl_ctrl()) << "\n"
       << senf::fieldName("adaptation field ctrl")     << "0x" << unsigned(p->adaptation_field_ctrl()) << "\n"
       << senf::fieldName("continuity counter")        << "0x" << unsigned(p->continuity_counter()) << "\n";
    if (p->pusi())
        os << senf::fieldName("pointer field")         << "0x" << unsigned(p->pointer_field()) << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
