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
        SENF_PARSER_BITFIELD   ( version,     2, unsigned );  // Version (RFC 3550)
        SENF_PARSER_BITFIELD   ( padding,     1, bool     );  // 1 if padding behind payload
        SENF_PARSER_BITFIELD   ( extension,   1, bool     );
        SENF_PARSER_BITFIELD_RO( csrcCount,   4, unsigned );  // 0-15,define the number of contributing sources
        SENF_PARSER_BITFIELD   ( marker,      1, bool     );  // Marker M=1, used to signal speech silent compression; further use in combination with PT to be defined
        SENF_PARSER_BITFIELD   ( payloadType, 7, unsigned );  // Payload Type; e.g. PCM=8 (RFC 3550)

        SENF_PARSER_FIELD( seqNumber,   UInt16Parser );  // random number to identify initial segment of  a stream, increment by 1 used to resequence segments at receiver
        SENF_PARSER_FIELD( timeStamp,   UInt32Parser );  // signals sampling time of 1st byte of payload; initialized; used to calculate Jitter between segments
        SENF_PARSER_FIELD( synSourceId, UInt32Parser );  // Synchronization source identifier; identifier of RFTP stream source (random number) in case of conferencing identifier of mixer

        SENF_PARSER_VECTOR( csrcOpt, csrcCount, UInt32Parser );

        SENF_PARSER_FINALIZE( RTPPacketParser );

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

    typedef ConcretePacket<RTPPacketType> RTPPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( RTPPacket );

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
