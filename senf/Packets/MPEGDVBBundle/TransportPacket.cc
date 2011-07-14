// $Id$
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

/** \file
    \brief TransportPacket non-inline non-template implementation */

#include "TransportPacket.hh"
//#include "TransportPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
