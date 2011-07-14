// $Id$
//
// Copyright (C) 2006
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
//   Philipp Batroff <pug@berlios.de>

/** \file
    \brief RTPPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_RTPPacket_
#define HH_SENF_Packets_DefaultBundle_RTPPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    struct RTPPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_BITFIELD  ( version,        2, unsigned     );      //Version (RFC 3550)
        SENF_PARSER_BITFIELD  ( padding,        1, bool         );      //1 if padding behind payload
        SENF_PARSER_BITFIELD  ( extension,      1, bool         );
        SENF_PARSER_BITFIELD_RO  ( csrcCount,      4, unsigned     );   //0-15,define the number of contributing sources
        SENF_PARSER_BITFIELD  ( marker,         1, bool         );      //Marker M=1, used to signal speech silent compression; further use in combination with PT to be defined
        SENF_PARSER_BITFIELD  ( payloadType,    7, unsigned     );      //Payload Type; e.g. PCM=8 (RFC 3550)
        SENF_PARSER_FIELD     ( seqNumber,      UInt16Parser );   //random number to identify initial segment of  a stream, increment by 1 used to resequence segments at receiver
        SENF_PARSER_FIELD     ( timeStamp,      UInt32Parser );   //signals sampling time of 1st byte of payload; initialized; used to calculate Jitter between segments
        SENF_PARSER_FIELD     ( synSourceId,    UInt32Parser );   //Synchronization source identifier; identifier of RFTP stream source (random number) in case of conferencing identifier of mixer
        SENF_PARSER_VECTOR    (csrcOpt, csrcCount, UInt32Parser );

        SENF_PARSER_FINALIZE(RTPPacketParser);

        bool valid() const { return version() == 2; }
    };

    struct RTPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<RTPPacketType>
    {
        typedef PacketTypeMixin<RTPPacketType> mixin;
        typedef ConcretePacket<RTPPacketType> packet;
        typedef RTPPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    typedef RTPPacketType::packet RTPPacket;

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
