//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief StringParser public header */

#ifndef HH_SENF_Packets_StringParser_
#define HH_SENF_Packets_StringParser_ 1

#ifndef HH_SENF_Packets_Packets_
#error "Don't include 'StringParser.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include "IntParser.hh"

//#include "StringParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template <class LengthParser=senf::UInt16Parser>
    class StringParser
        : public PacketParserBase
    {
    public:
        StringParser(data_iterator i, state_type s);

#       include SENF_PARSER()
        SENF_PARSER_FIELD_RO( length, LengthParser );

        typedef std::string value_type;
        static const size_type init_bytes = senf::init_bytes<LengthParser>::value;
        size_type bytes() const;

        value_type value() const;
        void value(value_type v);
        operator value_type() const;
        StringParser const & operator=(value_type other);

        bool empty() const;
        void clear();

        bool operator==(value_type const & other) const;
    };

    template <class LengthParser>
    std::ostream & operator<<(std::ostream & os, StringParser<LengthParser> const & value);


    template <std::string::size_type N>
    class FixedSizeStringParser
        : public PacketParserBase
    {
    public:
        FixedSizeStringParser(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        typedef std::string value_type;
        static size_type const fixed_bytes = N;

        value_type value() const;
        void value(value_type v);
        operator value_type() const;
        FixedSizeStringParser<N> const & operator=(value_type other);

        bool operator==(value_type const & other) const;
};

    template <std::string::size_type N>
    std::ostream & operator<<(std::ostream & os, FixedSizeStringParser<N> const & value);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_StringParser_i_)
#define HH_SENF_Packets_StringParser_i_
//#include "StringParser.cci"
//#include "StringParser.ct"
#include "StringParser.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
