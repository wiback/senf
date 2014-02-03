// $Id$
//
// Copyright (C) 2009
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
//   Stefan Bund <g0dil@berlios.de>
//   Thorsten Horstmann <tho@berlios.de>

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
