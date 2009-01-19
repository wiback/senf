// $Id: main.test.cc 206 2008-08-06 14:20:52Z pug $
//
// Copyright (C) 2006
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

// Definition of non-inline non-template functions

// Custom includes
#ifndef HH_SENF_Packets_DefaultBundle_RTPPacket_
#define HH_SENF_Packets_DefaultBundle_RTPPacket_

#include "../../Packets/Packets.hh"
#include "../../Socket/Protocols/INet/INet6Address.hh"
#include "../../Socket/Protocols/INet.hh"
#include "../../Utils/Logger/SenfLog.hh"

namespace senf {
    
    struct RTPPacketParser : public senf::PacketParserBase
    {
        SENF_LOG_CLASS_AREA();
#       include SENF_PARSER()
        SENF_PARSER_BITFIELD  ( version,        2, unsigned     );      //Version (RFC 3550)
        SENF_PARSER_BITFIELD  ( padding,        1, bool         );      //1 if padding behind payload
        SENF_PARSER_BITFIELD  ( extension,      1, bool         );
        SENF_PARSER_BITFIELD_RO  ( csrcCount,      4, unsigned     );   //0-15,define the number of contributing sources
        SENF_PARSER_BITFIELD  ( marker,         1, bool         );      //Marker M=1, used to signal speech silent compression; further use in combination with PT to be defined
        SENF_PARSER_BITFIELD  ( payloadType,    7, unsigned     );      //Payload Type; e.g. PCM=8 (RFC 3550)
        SENF_PARSER_FIELD     ( seqNumber,      senf::UInt16Parser );   //random number to identify initial segment of  a stream, increment by 1 used to resequence segments at receiver
        SENF_PARSER_FIELD     ( timeStamp,      senf::UInt32Parser );   //signals sampling time of 1st byte of payload; initialised; used to calculate Jitter between segments
        SENF_PARSER_FIELD     ( synSourceId,    senf::UInt32Parser );   //Synchronisation source identifier; identifier of RFTP stream source (random number) in case of conferencing identifier of mixer
        SENF_PARSER_VECTOR    (csrcOpt, csrcCount, senf::UInt32Parser );
        
        bool valid() const {return version() == 2;}
        SENF_PARSER_FINALIZE(RTPPacketParser);
    };
    
    struct RTPPacketType 
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<RTPPacketType>
    {
        SENF_LOG_CLASS_AREA();
        typedef senf::PacketTypeMixin<RTPPacketType> mixin;
        typedef senf::ConcretePacket<RTPPacketType> packet;
        typedef RTPPacketParser parser;
    
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
    
        static void dump(packet p, std::ostream &os);
    };
    
    typedef RTPPacketType::packet RTPPacket;

}
#endif
