// $Id$
//
// Copyright (C) 2013
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

/**file
   brief BitsetParser public header */

#ifndef HH_SENF_Packets_BitsetParser_
#define HH_SENF_Packets_BitsetParser_ 1

// Custom includes
#include <bitset>
#include <stdexcept>
#include <boost/integer/static_min_max.hpp>
#include "PacketParser.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct LSB0Endianness
    {
        static std::size_t bytePosition(std::size_t totalBytes, std::size_t bitPos);
        static std::size_t bit(std::size_t bitPos);
    };

    struct MSB0Endianness
    {
        static std::size_t bytePosition(std::size_t totalBytes, std::size_t bitPos);
        static std::size_t bit(std::size_t bitPos);
    };


    template <std::size_t N, typename Endianness>
    struct BitsetParser
        : public PacketParserBase
    {
        BitsetParser(data_iterator i, state_type s)
            : PacketParserBase(i,s,fixed_bytes) {}

        typedef std::bitset<N> value_type;
        static size_type const fixed_bytes = N/8 + boost::static_unsigned_min<1u, N % 8>::value;

        class reference
        {
        public:
            ~reference() {};
            reference & operator=(bool v);
            reference & operator=(reference const & other);
            bool operator~() const;
            operator bool() const;
            reference & flip();
        private:
            reference(BitsetParser<N,Endianness> & parser, std::size_t pos);
            BitsetParser<N,Endianness> & parser_;
            std::size_t pos_;
            friend struct BitsetParser<N,Endianness>;
        };

        void set();
        void set(std::size_t pos, bool v = true);

        void reset();
        void reset(std::size_t pos);

        void flip();
        void flip(std::size_t pos);

        bool any() const;
        bool none() const;
        bool all() const;

        bool test(std::size_t pos) const;

        bool operator[](std::size_t pos) const;
        reference operator[](std::size_t pos);

        std::size_t size() const;
        std::size_t count() const;

        void value(value_type const & bset);
        value_type value() const;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "BitsetParser.cci"
//#include "BitsetParser.ct"
#include "BitsetParser.cti"
#endif
