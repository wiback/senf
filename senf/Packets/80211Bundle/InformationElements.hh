// $Id$
//
// Copyright (C) 2009
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
    \brief WLAN Information Elements public header */

#ifndef HH_SENF_Packets_80211Bundle_InformationElements_
#define HH_SENF_Packets_80211Bundle_InformationElements_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "InformationElements.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class WLANInfoElementParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type,   UInt8Parser   );
        SENF_PARSER_FIELD    ( length, UInt8Parser   );
        SENF_PARSER_FINALIZE ( WLANInfoElementParser );
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
        // this strange information element hisself. 
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

///////////////////////////////hh.e////////////////////////////////////////
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
