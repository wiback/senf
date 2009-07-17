// $Id$
//
// Copyright (C) 2009
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

/** \file
    \brief MIH protocol non-inline non-template implementation */

#include "MIHPacket.hh"
//#include "MIHPacket.ih"

// Custom includes
#include "../../Packets/Packets.hh"
#include "../../Utils/hexdump.hh"
#include <boost/io/ios_state.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// MIHFId_TLVParser

prefix_ void senf::MIHFId_TLVParser::setString(std::string const &id)
{
    size_type str_size (id.size());
    // the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
    if (str_size > 253) 
        throw std::length_error("maximum length of a MIHF_ID is 253 octets");
    safe_data_iterator si = resizeValueField( str_size);   
    std::copy( id.begin(), id.end(), si);
}

prefix_ void senf::MIHFId_TLVParser::setMACAddress(senf::MACAddress const &mac)
{
    safe_data_iterator si = resizeValueField(12);
    std::copy( mac.begin(), mac.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFId_TLVParser::setINet4Address(senf::INet4Address const &addr)
{
    safe_data_iterator si = resizeValueField(8);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFId_TLVParser::setINet6Address(senf::INet6Address const &addr)
{
    safe_data_iterator si = resizeValueField(32);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFId_TLVParser::setEUI64(senf::EUI64 const &addr)
{
    safe_data_iterator si = resizeValueField(16);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ senf::MIHFId senf::MIHFId_TLVParser::valueAs(MIHFId::Type type)
    const
{
    if (length() == 0) return MIHFId();
    switch (type) {
    case MIHFId::Empty:
        return MIHFId();
    case MIHFId::MACAddress:
        return MIHFId( asMACAddress());
    case MIHFId::INet4Address:
        return MIHFId( asINet4Address());
    case MIHFId::INet6Address:
        return MIHFId( asINet6Address());
    case MIHFId::String:
        return MIHFId( asINet6Address());
    case MIHFId::EUI64:
        return MIHFId( asINet6Address());
    }
    return MIHFId();
}


///////////////////////////////////////////////////////////////////////////
// MIHPacketType

prefix_ void senf::MIHPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Packet:\n"
       << "  protocol header:\n"
       << "    Version:          " << unsigned( p->version()) << "\n"
       << "    Ack-Request:      " << p->ackRequest() << "\n"
       << "    Ack-Response:     " << p->ackResponse() << "\n"
       << "    UIR:              " << p->uir() << "\n"
       << "    more fragment:    " << p->moreFragment() << "\n"
       << "    fragment number:  " << p->fragmentNr() << "\n"
       << "    message ID (MID): " << unsigned( p->messageId()) << "\n"
       << "      SID:            " << unsigned( p->sid()) << "\n"        
       << "      Opcode:         " << unsigned( p->opcode()) << "\n"
       << "      AID:            " << unsigned( p->aid()) << "\n"      
       << "    Transaction ID:   " << unsigned( p->transactionId()) << "\n"
       << "    payload length:   " << unsigned( p->payloadLength()) << "\n"
       << "  source MIHF_Id TLV:\n"
       << "    type:             " << unsigned (p->src_mihfId().type()) << "\n"
       << "    length:           " << unsigned (p->src_mihfId().length()) << "\n"
       << "    value:\n";
    std::string src_mihfId (p->src_mihfId().asString());
    hexdump(src_mihfId.begin(), src_mihfId.end(), os);
    os << "  destination MIHF_Id TLV:\n"
       << "    type:             " << unsigned (p->dst_mihfId().type()) << "\n"
       << "    length:           " << unsigned (p->dst_mihfId().length()) << "\n"
       << "    value:\n";
    std::string dst_mihfId (p->dst_mihfId().asString());
    hexdump(dst_mihfId.begin(), dst_mihfId.end(), os);
}

prefix_ void senf::MIHPacketType::finalize(packet p)
{
    p->src_mihfId().finalizeLength();
    p->dst_mihfId().finalizeLength();
    p->payloadLength_() << p.size() - 8;
    p->messageId() << key(p.next(nothrow));
}

prefix_ senf::PacketInterpreterBase::factory_t senf::MIHPacketType::nextPacketType(packet p)
{
    if (p.data().size() < initSize())
        return no_factory();
    PkReg_Entry const * e (PacketRegistry<MIHMessageRegistry>::lookup( p->messageId(), nothrow ));
    return e ? e->factory() : MIHPayloadPacket::factory();
}

prefix_ void senf::MIHPayloadPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Payload (service specific TLVs):\n"
       << "  ToDo!\n";
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
