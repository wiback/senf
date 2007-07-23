// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief ParseInt public header */

#ifndef HH_ParseInt_
#define HH_ParseInt_ 1

// Custom includes
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include <boost/integer/integer_mask.hpp>
#include "PacketParser.hh"

//#include "ParseInt.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "ParseInt.ih"

namespace senf {

    /** \defgroup parseint Integer parsers

        Most packet fields will ultimately contain some type of integral number. The integer parsers
        allow to parse arbitrary integers in network byte order from 1-32 bit, both signed and
        unsigned. There are two types of integer parsers:

        \li The normal integer parsers with interpret 1-4 byte integers (9, 16, 24, 32 bits) aligned
            at byte boundaries.
        \li The bitfield parsers which parse integers with 1-32 bits aligned at any bit. A special
            case is the single bit flag parser.

        All fields are parsed in network byte order, the return value of all these parsers is the
        value in host byte order.

        The interface of all these parsers is the same (p is an arbitrary integer parser instance, v
        is an integer constant):

        \li <tt>p = v</tt>: Assigns the value to the packet field.
        \li <tt>p.value(v)</tt>: same as above.
        \li <tt>p.value()</tt>: Returns the fields value as an integer number.
        \li Use of p like an integer in most contexts: <tt>p += v</tt>, <tt>p *= v</tt>, <tt>v = p +
            1</tt> and so on. You will only need to use the explicit \c value() member in rare
            circumstances when the automatic conversion is ambiguous or in some template contexts.

        \ingroup packetparser
     */

    /** \brief Parse 8bit signed byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_Int8
        : public detail::packet::ParseIntOps<Parse_Int8,boost::int8_t>,
          public PacketParserBase
    {
        Parse_Int8(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int8_t value_type;
        static size_type const fixed_bytes = 1;

        value_type value() const { return i()[0]; }
        void value(value_type v) { i()[0] = v; }
        Parse_Int8 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_Int8
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_Int8 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 8bit unsigned byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_UInt8
        : public detail::packet::ParseIntOps<Parse_UInt8,boost::uint8_t>,
          public PacketParserBase
    {
        Parse_UInt8(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint8_t value_type;
        static size_type const fixed_bytes = 1;

        value_type value() const { return i()[0]; }
        void value(value_type v) { i()[0] = v; }
        Parse_UInt8 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_UInt8
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt8 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 16bit signed byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_Int16
        : public detail::packet::ParseIntOps<Parse_Int16,boost::int16_t>,
          public PacketParserBase
    {
        Parse_Int16(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int16_t value_type;
        static size_type const fixed_bytes = 2;

        value_type value() const { return detail::packet::parse_uint16(i()); }
        void value(value_type v) { detail::packet::write_uint16(i(),v); }
        Parse_Int16 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_Int16
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_Int16 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 16bit unsigned byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_UInt16
        : public detail::packet::ParseIntOps<Parse_UInt16,boost::uint16_t>,
          public PacketParserBase
    {
        Parse_UInt16(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint16_t value_type;
        static size_type const fixed_bytes = 2;

        value_type value() const { return detail::packet::parse_uint16(i()); }
        void value(value_type v) { detail::packet::write_uint16(i(),v); }
        Parse_UInt16 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_UInt16
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt16 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 24bit signed byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_Int24
        : public detail::packet::ParseIntOps<Parse_Int24,boost::int32_t>,
          public PacketParserBase
    {
        Parse_Int24(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;
        static size_type const fixed_bytes = 3;

        value_type value() const {
            value_type v (detail::packet::parse_uint24(i())); return v&0x800000 ? v|0xff000000 : v; }
        void value(value_type v) { detail::packet::write_uint24(i(),v); }
        Parse_Int24 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_Int24
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_Int24 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 24bit unsigned byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_UInt24
        : public detail::packet::ParseIntOps<Parse_UInt24,boost::uint32_t>,
          public PacketParserBase
    {
        Parse_UInt24(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;
        static size_type const fixed_bytes = 3;

        value_type value() const { return detail::packet::parse_uint24(i()); }
        void value(value_type v) { detail::packet::write_uint24(i(),v); }
        Parse_UInt24 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_UInt24
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt24 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 32bit signed byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_Int32
        : public detail::packet::ParseIntOps<Parse_Int32,boost::int32_t>,
          public PacketParserBase
    {
        Parse_Int32(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;
        static size_type const fixed_bytes = 4;

        value_type value() const { return detail::packet::parse_uint32(i()); }
        void value(value_type v) { detail::packet::write_uint32(i(),v); }
        Parse_Int32 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_Int32
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_Int32 const & i)
    { os << i.value(); return os; }

    /** \brief Parse 32bit unsigned byte aligned integer
        \see parseint
        \ingroup parseint
     */
    struct Parse_UInt32
        : public detail::packet::ParseIntOps<Parse_UInt32,boost::uint32_t>,
          public PacketParserBase
    {
        Parse_UInt32(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;
        static size_type const fixed_bytes = 4;

        value_type value() const { return detail::packet::parse_uint32(i()); }
        void value(value_type v) { detail::packet::write_uint32(i(),v); }
        Parse_UInt32 const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_UInt32
     */
    inline std::ostream & operator<<(std::ostream & os, Parse_UInt32 const & i)
    { os << i.value(); return os; }

    /** \brief Parse signed bitfield with up to 32bit's
        
        This parser will parse a bitfield beginning at the bit \a Start and ending \e before \a
        End. Bits are numbered <em>most significant bit first</em> as this is the customary
        numbering used when defining packet data structures. \a Start and \a End can be \e
        arbitrary as long as the field is between 1 and 32 bits in size. In other words, \c
        Parse_IntField<53,81> is a valid 30 bit field.

        When defining a compound parser with several bit fields, you need to take care of the fact,
        that several integer field parsers will interpret the same data \e bytes (but not the same
        \e bits). It is customary for several integer field parsers to start at the same byte offset
        with ever increasing bit offsets.

        \see parseint

        \implementation The integer field parser is highly optimized. Since the bit positions are
            compile-time constants, the compiler will create optimized bit-masks to directly access
            the value. The parser is also optimized to access the minimum number of data bytes
            necessary.
        
        \ingroup parseint
     */
    template <unsigned Start, unsigned End>
    struct Parse_IntField
        : public detail::packet::ParseIntOps<Parse_IntField<Start,End>,boost::int32_t>,
          public PacketParserBase
    {
        Parse_IntField(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;
        static size_type const fixed_bytes = (End-1)/8+1;

        value_type value() const {
            value_type v (detail::packet::parse_bitfield<Start,End>::parse(i()));
            return v&boost::high_bit_mask_t<End-Start-1>::high_bit ?
                v | ~boost::low_bits_mask_t<End-Start>::sig_bits : v;
        }
        void value(value_type v) { detail::packet::parse_bitfield<Start,End>::write(i(),v); }
        Parse_IntField const & operator= (value_type other) { value(other); return *this; }

    private:
        BOOST_STATIC_ASSERT( Start<End );
        BOOST_STATIC_ASSERT( End-Start<=32 );
    };
    /** \brief Write parsed value to stream
        \related Parse_IntField
     */
    template <unsigned Start, unsigned End>
    inline std::ostream & operator<<(std::ostream & os, Parse_IntField<Start,End> const & i)
    { os << i.value(); return os; }

    /** \brief Parse unsigned bitfield with up to 32bit's
        
        This parser will parse a bitfield beginning at the bit \a Start and ending \e before \a
        End. Bits are numbered <em>most significant bit first</em> as this is the customary
        numbering used when defining packet data structures. \a Start and \a End can be \e
        arbitrary as long as the field is between 1 and 32 bits in size. In other words, \c
        Parse_IntField<53,81> is a valid 30 bit field.

        When defining a compound parser with several bit fields, you need to take care of the fact,
        that several integer field parsers will interpret the same data \e bytes (but not the same
        \e bits). It is customary for several integer field parsers to start at the same byte offset
        with ever increasing bit offsets.

        \see parseint

        \implementation The integer field parser is highly optimized. Since the bit positions are
            compile-time constants, the compiler will create optimized bit-masks to directly access
            the value. The parser is also optimized to access the minimum number of data bytes
            necessary.
        
        \ingroup parseint
     */
    template <unsigned Start, unsigned End>
    struct Parse_UIntField
        : public detail::packet::ParseIntOps<Parse_UIntField<Start,End>,boost::uint32_t>,
          public PacketParserBase
    {
        Parse_UIntField(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;
        static size_type const fixed_bytes = (End-1)/8+1;

        value_type value() const { return detail::packet::parse_bitfield<Start,End>::parse(i()); }
        void value(value_type v) { detail::packet::parse_bitfield<Start,End>::write(i(),v); }
        Parse_UIntField const & operator= (value_type other) { value(other); return *this; }

    private:
        BOOST_STATIC_ASSERT( Start<End );
        BOOST_STATIC_ASSERT( End-Start<=32 );
    };
    /** \brief Write parsed value to stream
        \related Parse_UIntField
     */
    template <unsigned Start, unsigned End>
    inline std::ostream & operator<<(std::ostream & os, Parse_UIntField<Start,End> const & i)
    { os << i.value(); return os; }

    /** \brief Parse single-bit flag

        This parser will parse a single bit as True/False value. Bits are numbered <em>most
        significant bit first</em> as this is the customary numbering used when defining packet data
        structures. \a Bit can be arbitrary, \c Parse_Flag<75> is a valid flag parser.

        When defining a compound parser with several bit fields, you need to take care of the fact,
        that several integer field parsers will interpret the same data \e bytes (but not the same
        \e bits). It is customary for several integer field parsers to start at the same byte offset
        with ever increasing bit offsets.

        \see parseint
        \ingroup parseint
     */
    template <unsigned Bit>
    struct Parse_Flag
        : public detail::packet::ParseIntOps<Parse_Flag<Bit>,bool>,
          public PacketParserBase
    {
        Parse_Flag(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef bool value_type;
        static size_type const fixed_bytes = Bit/8+1;

        value_type value() const { return i()[Bit/8] & (1<<(7-(Bit%8))); }
        void value(value_type v) {
            if (v) i()[0] |= 1<<(7-(Bit%8));
            else   i()[0] &= ~(1<<(7-(Bit%8)));
        }
        Parse_Flag const & operator= (value_type other) { value(other); return *this; }
    };
    /** \brief Write parsed value to stream
        \related Parse_Flag
     */
    template <unsigned Bit>
    inline std::ostream & operator<<(std::ostream & os, Parse_Flag<Bit> const & i)
    { os << i.value(); return os; }

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(SENF_PACKETS_DECL_ONLY) && !defined(HH_ParseInt_i_)
#define HH_ParseInt_i_
//#include "ParseInt.cci"
//#include "ParseInt.ct"
//#include "ParseInt.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
