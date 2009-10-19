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
    \brief TLVParser non-inline non-template implementation */

#include "TLVParser.hh"
//#include "TLVParser.ih"

// Custom includes
#include <iomanip>
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/Format.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHFSrcIdTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHFDstIdTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHStatusTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHValidTimeIntervalTLVParser );

///////////////////////////////////////////////////////////////////////////
// senf::MIHFIdTLVParser

prefix_ void senf::MIHFIdTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "type:   " << unsigned (type()) << std::endl
       << indent << "length: " << unsigned (length()) << std::endl
       << indent << "value:\n";
    std::string src_mihfId (asString());
    hexdump(src_mihfId.begin(), src_mihfId.end(), os);
}

prefix_ void senf::MIHFIdTLVParser::setString(std::string const &id)
{
    size_type str_size (id.size());
    // the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
    if (str_size > 253) 
        throw std::length_error("maximum length of a MIHF_ID is 253 octets");
    safe_data_iterator si = resizeValueField( str_size);   
    std::copy( id.begin(), id.end(), si);
}

prefix_ void senf::MIHFIdTLVParser::setMACAddress(senf::MACAddress const &mac)
{
    safe_data_iterator si = resizeValueField(12);
    std::copy( mac.begin(), mac.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::setINet4Address(senf::INet4Address const &addr)
{
    safe_data_iterator si = resizeValueField(8);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::setINet6Address(senf::INet6Address const &addr)
{
    safe_data_iterator si = resizeValueField(32);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::setEUI64(senf::EUI64 const &addr)
{
    safe_data_iterator si = resizeValueField(16);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ senf::MIHFId senf::MIHFIdTLVParser::valueAs(MIHFId::Type type)
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
// senf::MIHFSrcIdTLVParser

prefix_ void senf::MIHFSrcIdTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "source MIHF_Id TLV:\n";
    MIHFIdTLVParser::dump(os);
}

///////////////////////////////////////////////////////////////////////////
// senf::MIHFDstIdTLVParser

prefix_ void senf::MIHFDstIdTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "destination MIHF_Id TLV:\n";
    MIHFIdTLVParser::dump(os);
}

///////////////////////////////////////////////////////////////////////////
// senf::MIHStatusTLVParser

prefix_ void senf::MIHStatusTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "Status TLV:" << std::endl;
    indent.increase();
    os << indent <<   "type:   " << unsigned( type()) << std::endl
       << indent <<   "length: " << unsigned( length()) << " byte(s)" << std::endl
       << indent <<   "value:  " << unsigned( value());
    switch (value()) {
    case Success:
        os << " (Success)" << std::endl;
        return;
    case UnspecifiedFailure:  
        os << " (Unspecified Failure)" << std::endl;
        return;
    case Rejected:
        os << " (Rejected)" << std::endl;
        return;
    case AuthorizationFailure:
        os << " (Authorization Failure)" << std::endl;
        return;
    case NetworkError:
        os << " (Network Error)" << std::endl;
        return;
    }
    os << " (???; invalid value!)" << std::endl;
}

///////////////////////////////////////////////////////////////////////////
// senf::MIHRegisterReqCodeTLVParser

prefix_ void senf::MIHRegisterReqCodeTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "Register Request Code TLV:" << std::endl;
    indent.increase();
    os << indent <<   "type:   " << unsigned( type()) << std::endl
       << indent <<   "length: " << unsigned( length()) << " byte(s)" << std::endl
       << indent <<   "value:  " << unsigned( value());
    switch (value()) {
    case Registration:
        os << " (Registration)" << std::endl;
        return;
    case ReRegistration:
        os << " (Re-Registration)" << std::endl;
        return;
    }
    os << " (???; invalid value!)" << std::endl;
}

///////////////////////////////////////////////////////////////////////////
// senf::MIHValidTimeIntervalTLVParser

prefix_ void senf::MIHValidTimeIntervalTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "Valid Time Interval TLV:" << std::endl;
    indent.increase();
    os << indent <<   "type:   " << unsigned( type()) << std::endl
       << indent <<   "length: " << unsigned( length()) << " byte(s)" << std::endl
       << indent <<   "value:  " << unsigned( value())
       << ( value()==0 ? " (infinite)" : " seconds") << std::endl;
}


///////////////////////////////////////////////////////////////////////////
// senf::MIHBaseTLVParser

prefix_ senf::safe_data_iterator senf::MIHBaseTLVParser::resizeValueField(
        MIHTLVLengthParser::value_type size) 
{
    MIHTLVLengthParser::value_type current_length ( length());
    length_() << size;

    safe_data_iterator si (data(), boost::next(i(), 1 + length_().bytes() ));
    if (current_length > size)
        data().erase( si, boost::next(si, current_length-size));
    else
        data().insert( si, size-current_length, 0);
    return si;
}


///////////////////////////////////////////////////////////////////////////
// senf::MIHTLVLengthParser

prefix_ senf::MIHTLVLengthParser::value_type senf::MIHTLVLengthParser::value() const 
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
        throw( MIHTLVLengthException());
    };
}

prefix_ void senf::MIHTLVLengthParser::value(value_type const & v) 
{
    switch (bytes() ) {
    case 1:
        if (v > 128) throw( MIHTLVLengthException());
        length_field() = v;
        return;
    case 2:
        if (v > UInt8Parser::max_value + 128) throw( MIHTLVLengthException());
        parse<UInt8Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    case 3:
        if (v > UInt16Parser::max_value + 128) throw( MIHTLVLengthException());
        parse<UInt16Parser>(1) = v - (v>128 ? 128 : 0);
        break;;
    case 4:
        if (v > UInt24Parser::max_value + 128) throw( MIHTLVLengthException());
        parse<UInt24Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    case 5:
        parse<UInt32Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    default:
        throw( MIHTLVLengthException());
    };
    underflow_flag() = (v <= 128);
}

prefix_ senf::MIHTLVLengthParser::value_type senf::MIHTLVLengthParser::maxValue()
    const
{
    switch (bytes() ) {
    case 1:
        return 128;
    case 2:
        return UInt8Parser::max_value + 128;
    case 3:
        return UInt16Parser::max_value + 128;
    case 4:
        return UInt24Parser::max_value + 128;
    case 5:
        return UInt32Parser::max_value; 
    default:
        throw( MIHTLVLengthException());
    };
}

prefix_ senf::MIHTLVLengthParser const & senf::MIHTLVLengthParser::operator= (value_type other) 
{
    value(other);
    return *this; 
}

prefix_ void senf::MIHTLVLengthParser::init() const 
{
    defaultInit();
    extended_length_flag() = false;
}

prefix_ void senf::MIHTLVLengthParser::finalize()
{
    value_type v = value();
    size_type b = bytes();
    if (v <= 128) {
        if (b != 1) resize_(1);
        return;
    }
    if (v <= UInt8Parser::max_value + 128) {
        if (b != 2) resize_(2);
        return;
    }
    if (v <= UInt16Parser::max_value + 128) {
        if (b != 3) resize_(3);
        return;
    }
    if (v <= UInt24Parser::max_value + 128 ) {
        if (b != 4) resize_(4);
        return;
    }
    if (b != 5) resize_(5);
}

prefix_ void senf::MIHTLVLengthParser:: maxValue(MIHTLVLengthParser::value_type v)
{
    if (v <= 128)
        return;
    size_type b = bytes();
    if (v <= UInt8Parser::max_value + 128) {
        if (b < 2) resize_(2);
        return;
    }
    if (v <= UInt16Parser::max_value + 128) {
        if (b < 3) resize_(3);
        return;
    }
    if (v <= UInt24Parser::max_value + 128) {
        if (b < 4) resize_(4);
        return;
    }
    if (b < 5) resize_(5);
}

prefix_ void senf::MIHTLVLengthParser::resize_(size_type size)
{
    value_type v = value();
    resize(bytes(), size);
    if (size > 1) {
        extended_length_flag() = true;
        fixed_length_field() = size - 1;
    } else {
        extended_length_flag() = false;
    }
    value(v);
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
