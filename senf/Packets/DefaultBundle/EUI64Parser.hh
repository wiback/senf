// $Id$
//
// Copyright (C) 2010
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
    \brief EUI64Parser public header */

#ifndef HH_SENF_Packets_DefaultBundle_EUI64Parser_
#define HH_SENF_Packets_DefaultBundle_EUI64Parser_ 1

// Custom includes
#include <senf/Packets.hh>
#include <senf/Socket/Protocols/Raw/EUI64.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    /** \brief Parse an EUI64 address

        The EUI64 address is returned by value as a 8-byte sequence

        \see EUI64
     */
    struct EUI64Parser
        : public ValueParserBase<EUI64Parser, EUI64, 8u>
    {
        EUI64Parser(data_iterator i, state_type s) : Base(i,s) {}

        value_type value() const { return value_type::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }

        using Base::operator=;
    };

}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EUI64Parser.cci"
//#include "EUI64Parser.ct"
//#include "EUI64Parser.cti"
#endif
