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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief WLAN Information Elements public header */

#ifndef HH_SENF_Packets_80211Bundle_InformationElements_
#define HH_SENF_Packets_80211Bundle_InformationElements_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "InformationElements.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class WLANInfoElementParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type,   UInt8Parser   );
        SENF_PARSER_FIELD    ( length, UInt8Parser   );
        SENF_PARSER_FINALIZE ( WLANInfoElementParser );

        typedef GenericTLVParserRegistry<WLANInfoElementParser> Registry;
    };

    typedef GenericTLVParserBase<WLANInfoElementParser> WLANGenericInfoElementParser;

    struct WLANSSIDInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( WLANInfoElementParser            );
        // the StringParser includes the length field so we have to go back
        SENF_PARSER_GOTO     ( length                           );
        SENF_PARSER_FIELD    ( value, StringParser<UInt8Parser> );
        SENF_PARSER_FINALIZE ( WLANSSIDInfoElementParser        );

        SENF_PARSER_INIT() {
            type() = typeId;
        }
        static const type_t::value_type typeId = 0x00u;

        void dump(std::ostream & os) const;
    };

    struct WLANSupportedRatesInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( WLANInfoElementParser               );
        // we just skip the value here. If somebody needs the information he has to implement
        // this strange information element himself.
        SENF_PARSER_SKIP     ( length(), 0                         );
        SENF_PARSER_FINALIZE ( WLANSupportedRatesInfoElementParser );

        SENF_PARSER_INIT() {
            type() = typeId;
        }
        static const type_t::value_type typeId = 0x01u;

        void dump(std::ostream & os) const;
    };

    struct WLANPowerConstraintInfoElementParser
        : public WLANInfoElementParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( WLANInfoElementParser                );
        SENF_PARSER_FIELD    ( value,   UInt8Parser                 );
        SENF_PARSER_FINALIZE ( WLANPowerConstraintInfoElementParser );

        SENF_PARSER_INIT() {
            type() = typeId;
            length() = 1;
        }
        static const type_t::value_type typeId = 0x20u;

        void dump(std::ostream & os) const;
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "InformationElements.cci"
//#include "InformationElements.ct"
//#include "InformationElements.cti"
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
