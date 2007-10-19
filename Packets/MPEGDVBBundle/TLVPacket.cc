// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
#include <senf/Utils/hexdump.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::Parse_TLVPacketLength::value_type senf::Parse_TLVPacketLength::value() const 
{
    switch (bytes() ) {
    case 1:
        return fixed_length_field().value();
    case 2:
        return parse<Parse_UInt8>( 1 ).value();
    case 3:
        return parse<Parse_UInt16>( 1 ).value();
    case 4:
        return parse<Parse_UInt24>( 1 ).value();
    case 5:
        return parse<Parse_UInt32>( 1 ).value();
    default:
        throw(UnsuportedTLVPacketException());
    };
}

prefix_ void senf::Parse_TLVPacketLength::value(value_type const & v) 
{
    if (v > 4294967295u)
        throw(UnsuportedTLVPacketException());
    
    SafePacketParser<Parse_TLVPacketLength> safeThis (*this);
    if (v < 128u) {
        if (bytes() != 1) {
            resize(1);
            safeThis->extended_length_flag() = false;
        }
        safeThis->fixed_length_field() = v;
        return;
    }
    if (v < 256u) {
        if (bytes() != 2) {
	    resize(2);
            safeThis->extended_length_flag() = true;
	    safeThis->fixed_length_field() = 1;
	}
        safeThis->parse<Parse_UInt8>(1) = v;
        return;
    }
    if (v < 65536u) {
        if (bytes() != 3) {
	    resize(3);
	    safeThis->extended_length_flag() = true;
	    safeThis->fixed_length_field() = 2;
	}
        safeThis->parse<Parse_UInt16>(1) = v;
        return;
    }
    if (v < 16777216u) {
        if (bytes() != 4) {
	    resize(4);
	    safeThis->extended_length_flag() = true;
	    safeThis->fixed_length_field() = 3;
	}
        safeThis->parse<Parse_UInt24>(1) = v;
        return;
    }
    if (v <= 4294967295u) {
        if (bytes() != 5) {
	    resize(5);
	    safeThis->extended_length_flag() = true;
	    safeThis->fixed_length_field() = 4;
	}
        safeThis->parse<Parse_UInt32>(1) = v;
        return;
    }
}

prefix_ senf::Parse_TLVPacketLength const & senf::Parse_TLVPacketLength::operator= (value_type other) 
{
    value(other);
    return *this; 
}

prefix_ senf::Parse_TLVPacketLength::size_type senf::Parse_TLVPacketLength::bytes() const 
{
    if ( extended_length_flag() )
        return 1 + fixed_length_field();
    else
        return 1;
}
    
prefix_ void senf::Parse_TLVPacketLength::init() const 
{
    defaultInit();
    extended_length_flag() = 0;
}

prefix_ void senf::Parse_TLVPacketLength::resize(size_type size) 
{
    size_type current_size (bytes());
    safe_data_iterator si (data(), i());
    
    if (current_size > size)
        data().erase( si, boost::next(si, current_size-size));
    else
        data().insert( si, size-current_size, 0);
}

prefix_ void senf::TLVPacketType::dump(packet p, std::ostream & os)
{
    os << "TLVPacket:\n"
       << std::dec
       << "  type: " <<  unsigned(p->type()) << "\n"
       << "  length: " << unsigned(p->length()) << "\n";
}

prefix_ senf::PacketParserBase::size_type senf::TLVPacketType::initSize()
{
    return 5;  // 4 bytes type + 1 byte length
}

prefix_ void senf::TLVPacketType::init(packet p)
{
    p->init();
}

prefix_ void senf::TLVPacketType::finalize(packet p)
{
    p->length() = p.next().data().size();
}

prefix_ senf::PacketInterpreterBase::optional_range 
senf::TLVPacketType::nextPacketRange(packet p) 
{
    if (p.data().size() < 5)
        return no_range();
    return range(
            boost::next(p.data().begin(), 4 + senf::bytes(p->length()) ),
            p.data().end() );
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
