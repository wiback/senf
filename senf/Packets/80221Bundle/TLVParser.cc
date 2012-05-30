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
    \brief TLVParser non-inline non-template implementation */

#include "TLVParser.hh"
//#include "TLVParser.ih"

// Custom includes
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/Format.hh>
#include "Exceptions.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHFSrcIdTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHFDstIdTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHStatusTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::MIHValidTimeIntervalTLVParser );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHFIdTLVParser

prefix_ void senf::MIHFIdTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "type:   " << unsigned (type()) << std::endl
       << indent << "length: " << unsigned (length()) << std::endl
       << indent << "value:\n";
    std::string src_mihfId (valueAsString());
    hexdump(src_mihfId.begin(), src_mihfId.end(), os);
}

prefix_ void senf::MIHFIdTLVParser::finalize()
{
    protect(), idLength_().finalize();
    length_() << idLength() + senf::bytes(idLength_());
    MIHBaseTLVParser::finalize();
}

prefix_ void senf::MIHFIdTLVParser::maxIdLength(boost::uint8_t maxl)
    const
{
    // the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
    if (maxl > 253)
        throw std::length_error("maximum length of a MIHF_ID is 253 octets");
    protect(), idLength_().capacity( maxl);
    maxLength( maxl + senf::bytes(idLength_()));
}

prefix_ senf::safe_data_iterator senf::MIHFIdTLVParser::resizeValueField(
        MIHLengthParser::value_type size)
{
    MIHLengthParser::value_type current_length ( idLength());
    idLength_() << size;
    length_() << size + idLength_().bytes();

    safe_data_iterator si (data(), valueBegin());
    if (current_length > size)
        data().erase( si, boost::next(si, current_length-size));
    else
        data().insert( si, size-current_length, 0);
    return si;
}

prefix_ void senf::MIHFIdTLVParser::value(std::string const & id)
{
    size_type str_size (id.size());
    // the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
    if (str_size > 253)
        throw std::length_error("maximum length of a MIHF_ID is 253 octets");
    safe_data_iterator si = resizeValueField( str_size);
    std::copy( id.begin(), id.end(), si);
}

prefix_ void senf::MIHFIdTLVParser::value(senf::MACAddress const & addr)
{
    safe_data_iterator si = resizeValueField(6*2);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::value(senf::INet4Address const & addr)
{
    safe_data_iterator si = resizeValueField(4*2);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::value(senf::INet6Address const & addr)
{
    safe_data_iterator si = resizeValueField(16*2);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::value(senf::EUI64 const & addr)
{
    safe_data_iterator si = resizeValueField(8*2);
    std::copy( addr.begin(), addr.end(), getNAIEncodedOutputIterator(si));
}

prefix_ void senf::MIHFIdTLVParser::value( MIHFId const & id)
{
    boost::apply_visitor( ValueSetterVisitor(*this), id);
}

prefix_ senf::MIHFId senf::MIHFIdTLVParser::valueAs(MIHFId::Type type)
    const
{
    if (length() == 0) return MIHFId();
    switch (type) {
    case MIHFId::MulticastType:
        return MIHFId();
    case MIHFId::MACAddress:
        return MIHFId( valueAsMACAddress());
    case MIHFId::INet4Address:
        return MIHFId( valueAsINet4Address());
    case MIHFId::INet6Address:
        return MIHFId( valueAsINet6Address());
    case MIHFId::String:
        return MIHFId( valueAsString());
    case MIHFId::EUI64:
        return MIHFId( valueAsEUI64());
    }
    return MIHFId();
}

prefix_ void senf::swap(MIHFSrcIdTLVParser src, MIHFDstIdTLVParser dst)
{
    std::string d (dst.valueAsString());
    SafePacketParserWrapper<MIHFSrcIdTLVParser> safe_src (src);
    dst << src.valueAsString();
    *safe_src << d;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHFSrcIdTLVParser

prefix_ void senf::MIHFSrcIdTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "source MIHF_Id TLV:\n";
    MIHFIdTLVParser::dump(os);
}

prefix_ void senf::MIHFSrcIdTLVParser::validate()
    const
{
    validateType( typeId);
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHFDstIdTLVParser

prefix_ void senf::MIHFDstIdTLVParser::dump(std::ostream & os)
    const
{
    senf::format::IndentHelper indent;
    os << indent << "destination MIHF_Id TLV:\n";
    MIHFIdTLVParser::dump(os);
}

prefix_ void senf::MIHFDstIdTLVParser::validate()
    const
{
    validateType( typeId);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

prefix_ void senf::MIHStatusTLVParser::validate()
    const
{
    validateTypeLength( typeId, 1);
    if (value() >= 4)
        throw InvalidMIHPacketException("invalid value in MIHStatusTLV ") << unsigned( value());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

prefix_ void senf::MIHRegisterReqCodeTLVParser::validate()
    const
{
    validateTypeLength( typeId, 1);
    if (value() >= 2)
        throw InvalidMIHPacketException("invalid value in MIHRegisterReqCodeTLV ") << unsigned( value());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

prefix_ void senf::MIHValidTimeIntervalTLVParser::validate()
    const
{
    validateTypeLength( typeId, 4);
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
