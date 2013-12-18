// $Id$
//
// Copyright (C) 2010
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
        : public ArrayValueParserBase<EUI64Parser, EUI64>
    {
        EUI64Parser(data_iterator i, state_type s) : Base(i,s) {}

        using Base::operator=;
    };

}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EUI64Parser.cci"
//#include "EUI64Parser.ct"
//#include "EUI64Parser.cti"
#endif
