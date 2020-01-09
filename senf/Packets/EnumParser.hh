// $Id$
//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.fit.fraunhofer.de/license.html
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
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/**file
   brief EnumParser public header */

#ifndef HH_SENF_Packets_EnumParser_
#define HH_SENF_Packets_EnumParser_ 1

// Custom includes
#include "PacketParser.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template <typename EnumType, typename UnderlyingTypeParser>
    class EnumParser
        : public PacketParserBase
    {
    public:
        EnumParser(data_iterator i, state_type s);

        typedef EnumType value_type;
        static size_type const fixed_bytes = UnderlyingTypeParser::fixed_bytes;

        void value(EnumType v);
        EnumType value() const;

    private:
        UnderlyingTypeParser underlyingValue() const;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "EnumParser.cci"
//#include "EnumParser.ct"
#include "EnumParser.cti"
#endif
