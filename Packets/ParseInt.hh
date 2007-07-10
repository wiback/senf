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

#ifndef HH_ParseInt_
#define HH_ParseInt_ 1

// Custom includes
#include <iostream>
#include "ParserBase.hh"
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include <boost/integer/integer_mask.hpp>

//#include "ParseInt.mpp"
///////////////////////////////hh.p////////////////////////////////////////
#include "ParseInt.ih"

namespace senf {


    template <class Iterator=nil, class IPacket=nil>
    struct Parse_Int8
        : public impl::ParseIntOps<Parse_Int8<Iterator,IPacket>,boost::int8_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Int8<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 1; }

        Parse_Int8() {}
        explicit Parse_Int8(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int8_t value_type;

        value_type value() const { return this->i()[0]; }
        void value(value_type v) { this->i()[0] = v; }
        Parse_Int8 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_Int8<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_UInt8
        : public impl::ParseIntOps<Parse_UInt8<Iterator,IPacket>,boost::uint8_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_UInt8<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 1; }

        Parse_UInt8() {}
        explicit Parse_UInt8(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint8_t value_type;

        value_type value() const { return this->i()[0]; }
        void value(value_type v) { this->i()[0] = v; }
        Parse_UInt8 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_UInt8<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_Int16
        : public impl::ParseIntOps<Parse_Int16<Iterator,IPacket>,boost::int16_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Int16<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 2; }

        Parse_Int16() {}
        explicit Parse_Int16(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int16_t value_type;

        value_type value() const { return impl::parse_uint16(this->i()); }
        void value(value_type v) { impl::write_uint16(this->i(),v); }
        Parse_Int16 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_Int16<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_UInt16
        : public impl::ParseIntOps<Parse_UInt16<Iterator,IPacket>,boost::uint16_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_UInt16<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 2; }

        Parse_UInt16() {}
        explicit Parse_UInt16(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint16_t value_type;

        value_type value() const { return impl::parse_uint16(this->i()); }
        void value(value_type v) { impl::write_uint16(this->i(),v); }
        Parse_UInt16 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_UInt16<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_Int24
        : public impl::ParseIntOps<Parse_Int24<Iterator,IPacket>,boost::int32_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Int24<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 3; }

        Parse_Int24() {}
        explicit Parse_Int24(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;

        value_type value() const {
            value_type v (impl::parse_uint24(this->i())); return v&0x800000 ? v|0xff000000 : v; }
        void value(value_type v) { impl::write_uint24(this->i(),v); }
        Parse_Int24 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_Int24<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_UInt24
        : public impl::ParseIntOps<Parse_UInt24<Iterator,IPacket>,boost::uint32_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_UInt24<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 3; }

        Parse_UInt24() {}
        explicit Parse_UInt24(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;

        value_type value() const { return impl::parse_uint24(this->i()); }
        void value(value_type v) { impl::write_uint24(this->i(),v); }
        Parse_UInt24 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_UInt24<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_Int32
        : public impl::ParseIntOps<Parse_Int32<Iterator,IPacket>,boost::int32_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Int32<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 4; }

        Parse_Int32() {}
        explicit Parse_Int32(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;

        value_type value() const { return impl::parse_uint32(this->i()); }
        void value(value_type v) { impl::write_uint32(this->i(),v); }
        Parse_Int32 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_Int32<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <class Iterator=nil, class IPacket=nil>
    struct Parse_UInt32
        : public impl::ParseIntOps<Parse_UInt32<Iterator,IPacket>,boost::uint32_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_UInt32<I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 4; }

        Parse_UInt32() {}
        explicit Parse_UInt32(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;

        value_type value() const { return impl::parse_uint32(this->i()); }
        void value(value_type v) { impl::write_uint32(this->i(),v); }
        Parse_UInt32 const & operator= (value_type other) { value(other); return *this; }
    };
    template <class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_UInt32<Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <unsigned start, unsigned end, class Iterator=nil, class IPacket=nil>
    struct Parse_IntField
        : public impl::ParseIntOps<Parse_IntField<start,end,Iterator,IPacket>,boost::int32_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_IntField<start,end,I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return (end-1)/8+1; }

        Parse_IntField() {}
        explicit Parse_IntField(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::int32_t value_type;

        value_type value() const {
            value_type v (impl::parse_bitfield<Iterator,start,end>::parse(this->i()));
            return v&boost::high_bit_mask_t<end-start-1>::high_bit ?
                v | ~boost::low_bits_mask_t<end-start>::sig_bits : v;
        }
        void value(value_type v) { impl::parse_bitfield<Iterator,start,end>::write(this->i(),v); }
        Parse_IntField const & operator= (value_type other) { value(other); return *this; }

    private:
        BOOST_STATIC_ASSERT( start<end );
        BOOST_STATIC_ASSERT( end-start<=32 );
    };
    template <unsigned start, unsigned end, class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_IntField<start,end,Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <unsigned start, unsigned end, class Iterator=nil, class IPacket=nil>
    struct Parse_UIntField
        : public impl::ParseIntOps<Parse_UIntField<start,end,Iterator,IPacket>,boost::uint32_t>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_UIntField<start,end,I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return (end-1)/8+1; }

        Parse_UIntField() {}
        explicit Parse_UIntField(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef boost::uint32_t value_type;

        value_type value() const { return impl::parse_bitfield<Iterator,start,end>::parse(this->i()); }
        void value(value_type v) { impl::parse_bitfield<Iterator,start,end>::write(this->i(),v); }
        Parse_UIntField const & operator= (value_type other) { value(other); return *this; }

    private:
        BOOST_STATIC_ASSERT( start<end );
        BOOST_STATIC_ASSERT( end-start<=32 );
    };
    template <unsigned start, unsigned end, class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_UIntField<start,end,Iterator,IPacket> const & i)
    { os << i.value(); return os; }

    template <unsigned bit, class Iterator=nil, class IPacket=nil>
    struct Parse_Flag
        : public impl::ParseIntOps<Parse_Flag<bit,Iterator,IPacket>,bool>,
          public ParserBase<Iterator,IPacket>
    {
        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Flag<bit,I,P> parser; };
        typedef Iterator byte_iterator;

        static unsigned bytes() { return 1; }

        Parse_Flag() {}
        explicit Parse_Flag(Iterator const & i) : ParserBase<Iterator,IPacket>(i) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef bool value_type;

        value_type value() const { return this->i()[bit/8] & (1<<(7-(bit%8))); }
        void value(value_type v) {
            if (v) this->i()[0] |= 1<<(7-(bit%8));
            else   this->i()[0] &= ~(1<<(7-(bit%8)));
        }
        Parse_Flag const & operator= (value_type other) { value(other); return *this; }
    };
    template <unsigned bit, class Iterator, class IPacket>
    std::ostream & operator<<(std::ostream & os, Parse_Flag<bit,Iterator,IPacket> const & i)
    { os << i.value(); return os; }

}

///////////////////////////////hh.e////////////////////////////////////////
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
