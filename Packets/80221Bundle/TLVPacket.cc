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

/** \file
    \brief TLVPacket non-inline non-template implementation */

#include "TLVPacket.hh"
//#include "TLVPacket.ih"

// Custom includes
#include <iomanip>
#include "../../Utils/hexdump.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::safe_data_iterator senf::BaseTLVPacketParser::resizeValueField(
        DynamicTLVLengthParser::value_type size) 
{
    DynamicTLVLengthParser::value_type current_length ( length());
    length( size);

    safe_data_iterator si (data(), boost::next(i(), 1 + length_bytes() ));
    if (current_length > size)
        data().erase( si, boost::next(si, current_length-size));
    else
        data().insert( si, size-current_length, 0);
    return si;
}


prefix_ senf::DynamicTLVLengthParser::value_type senf::DynamicTLVLengthParser::value() const 
{
    switch (bytes() ) {
    case 1:
        return length_field().value();
    case 2:
        return parse<UInt8Parser>( 1 ).value() + (underflow_flag() ? 0 : 128u);
    case 3:
        return parse<UInt16Parser>( 1 ).value() + (underflow_flag() ? 0 : 128u);
    case 4:
        return parse<UInt24Parser>( 1 ).value() + (underflow_flag() ? 0 : 128u);
    case 5:
        return parse<UInt32Parser>( 1 ).value() + (underflow_flag() ? 0 : 128u);
    default:
        throw(TLVLengthException());
    };
}


prefix_ void senf::DynamicTLVLengthParser::value(value_type const & v) 
{
    switch (bytes() ) {
    case 1:
        if (v > 128) throw( TLVLengthException());
        length_field() = v;
        return;
    case 2:
        if (v > UInt8Parser::max_value + 128) throw( TLVLengthException());
        parse<UInt8Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    case 3:
        if (v > UInt16Parser::max_value + 128) throw( TLVLengthException());
        parse<UInt16Parser>(1) = v - (v>128 ? 128 : 0);
        break;;
    case 4:
        if (v > UInt24Parser::max_value + 128) throw( TLVLengthException());
        parse<UInt24Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    case 5:
        parse<UInt32Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    default:
        throw( TLVLengthException());
    };
    underflow_flag() = v < 128;
}


prefix_ senf::DynamicTLVLengthParser const & senf::DynamicTLVLengthParser::operator= (value_type other) 
{
    value(other);
    return *this; 
}


prefix_ void senf::DynamicTLVLengthParser::init() const 
{
    defaultInit();
    extended_length_flag() = false;
}


prefix_ void senf::DynamicTLVLengthParser::shrink()
{
    value_type v = value();
    size_type b = bytes();
    if (v <= 128) {
        if (b != 1) resize(1);
        return;
    }
    if (v <= UInt8Parser::max_value + 128) {
        if (b != 2) resize(2);
        return;
    }
    if (v <= UInt16Parser::max_value + 128) {
        if (b != 3) resize(3);
        return;
    }
    if (v <= UInt24Parser::max_value + 128 ){
        if (b != 4) resize(4);
        return;
    }
    if (b != 5) resize(5);
}


prefix_ void senf::DynamicTLVLengthParser:: maxValue(DynamicTLVLengthParser::value_type v)
{
    if (v <= 128)
        return;
    size_type b = bytes();
    if (v <= UInt8Parser::max_value + 128) {
        if (b < 2) resize(2);
        return;
    }
    if (v <= UInt16Parser::max_value + 128) {
        if (b < 3) resize(3);
        return;
    }
    if (v <= UInt24Parser::max_value + 128) {
        if (b < 4) resize(4);
        return;
    }
    if (b < 5) resize(5);
}


prefix_ void senf::DynamicTLVLengthParser::resize(size_type size)
{
    value_type v = value();
    size_type current_size (bytes());
    SafePacketParserWrapper<DynamicTLVLengthParser> safeThis (*this);
    
    if (current_size > size)
        data().erase( i(), boost::next(i(), current_size-size));
    else
        data().insert( i(), size-current_size, 0);
    
    if (size > 1) {
        safeThis->extended_length_flag() = true;
        safeThis->fixed_length_field() = size - 1;
    } else {
        safeThis->extended_length_flag() = false;
    }
    safeThis->value(v);
}


prefix_ senf::PacketInterpreterBase::range senf::GenericTLVPacketParser::value() 
    const
{
    senf::PacketData::iterator begin (boost::next(data().begin(), 1 + length_bytes() ));
    return PacketInterpreterBase::range(
            begin, boost::next( begin, length()) );
}


prefix_ void senf::GenericTLVPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "GenericTLVPacket:\n"
       << std::dec
       << "  type:   " << unsigned( p->type()) << "\n"
       << "  length: " << unsigned( p->length()) << "\n"
       << "  value\n:";
    senf::hexdump( p->value().begin(), p->value().end(), os);
}


prefix_ void senf::GenericTLVPacketType::finalize(packet p)
{
    p->shrinkLength();
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
