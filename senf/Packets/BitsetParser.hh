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
