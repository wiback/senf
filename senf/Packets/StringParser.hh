// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

        //-////////////////////////////////////////////////////////////////////////

#       include SENF_PARSER()
        SENF_PARSER_PRIVATE_FIELD ( length, LengthParser );

        typedef std::string value_type;
        static const size_type init_bytes = senf::init_bytes<LengthParser>::value;
        size_type bytes() const;

        value_type value() const;
        void value(value_type v);
        operator value_type() const;
        StringParser const & operator=(value_type other);
    };

    template <class LengthParser>
    std::ostream & operator<<(std::ostream & os, StringParser<LengthParser> const & value);

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
