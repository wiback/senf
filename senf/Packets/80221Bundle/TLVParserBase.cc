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
    \brief TLVParserBase non-inline non-template implementation */

#include "TLVParserBase.hh"

// Custom includes
#include "Exceptions.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// MIHBaseTLVParser

prefix_ void senf::MIHBaseTLVParser::validateType(boost::uint8_t expectedType)
    const
{
    if (! check( 1 + senf::bytes(length_()) + length()) )
        throw InvalidMIHPacketException("truncated TLV.") << " Type: " << unsigned(type())
            << ", size:" << 1 + senf::bytes(length_()) + length()
            << ", available: " << std::distance(i(), data().end());
    if (type() != expectedType)
        throw InvalidMIHPacketException("wrong TLV type. expected ") << unsigned(expectedType) << " got " << unsigned(type());
}

prefix_ void senf::MIHBaseTLVParser::validateTypeLength(boost::uint8_t expectedType, MIHLengthParser::value_type expectedLength)
    const
{
    validateType( expectedType);
    if (length() != expectedLength)
        throw InvalidMIHPacketException("invalid length in TLV.") << " Type: " << unsigned(type())
                << ", expected length: " << expectedLength << " got " << length();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHBaseListTLVParser

prefix_ void senf::MIHBaseListTLVParser::maxListSize(MIHLengthParser::value_type maxl)
    const
{
    protect(), listSize_().capacity( maxl);
    maxLength( maxl + senf::bytes(listSize_()));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHLengthParser

prefix_ senf::MIHLengthParser::value_type senf::MIHLengthParser::value() const
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
        throw( MIHLengthException("cannot get length value: size of length field is greater than 5: ") << bytes());
    }
}

prefix_ void senf::MIHLengthParser::value(value_type v)
{
    switch (bytes() ) {
    case 1:
        if (v > 128)
            throw( MIHLengthException("cannot set length to ") << v << ": insufficient size of length field " << bytes());
        length_field() = v;
        return;
    case 2:
        if (v > UInt8Parser::max_value + 128)
            throw( MIHLengthException("cannot set length to ") << v << ": insufficient size of length field " << bytes());
        parse<UInt8Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    case 3:
        if (v > UInt16Parser::max_value + 128)
            throw( MIHLengthException("cannot set length to ") << v << ": insufficient size of length field " << bytes());
        parse<UInt16Parser>(1) = v - (v>128 ? 128 : 0);
        break;;
    case 4:
        if (v > UInt24Parser::max_value + 128)
            throw( MIHLengthException("cannot set length to ") << v << ": insufficient size of length field " << bytes());
        parse<UInt24Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    case 5:
        parse<UInt32Parser>(1) = v - (v>128 ? 128 : 0);
        break;
    default:
        throw( MIHLengthException("cannot set length to ") << v << ": size of length field is greater than 5: " << bytes());
    };
    underflow_flag() = (v <= 128);
}

prefix_ senf::MIHLengthParser::value_type senf::MIHLengthParser::capacity()
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
        throw( MIHLengthException("cannot get capacity: size of length field is greater than 5: ") << bytes());
    };
}

prefix_ senf::MIHLengthParser const & senf::MIHLengthParser::operator= (value_type other)
{
    value(other);
    return *this;
}

prefix_ void senf::MIHLengthParser::init() const
{
    defaultInit();
    extended_length_flag() = false;
}

prefix_ void senf::MIHLengthParser::finalize()
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

prefix_ void senf::MIHLengthParser::capacity(MIHLengthParser::value_type v)
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

prefix_ void senf::MIHLengthParser::resize_(size_type size)
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
