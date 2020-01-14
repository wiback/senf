//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#ifndef HH_WiBACK_DataPlane_Packets_InternalThroughputTestPacket_
#define HH_WiBACK_DataPlane_Packets_InternalThroughputTestPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace emu {

    struct InternalThroughputTestPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD( sessionId, senf::UInt32Parser );
        SENF_PARSER_FIELD( seqNo,     senf::UInt32Parser );
        SENF_PARSER_FIELD( numPkts,   senf::UInt32Parser );
        SENF_PARSER_FIELD( magic,     senf::UInt64Parser );
        SENF_PARSER_FIELD( timestamp, senf::UInt64Parser );
        SENF_PARSER_FINALIZE( InternalThroughputTestPacketParser );
    };

    struct InternalThroughputTestPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<InternalThroughputTestPacketType>
    {
        typedef senf::PacketTypeMixin<InternalThroughputTestPacketType> mixin;
        typedef senf::ConcretePacket<InternalThroughputTestPacketType> packet;
        typedef InternalThroughputTestPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static void dump(packet p, std::ostream & os);

        static const std::uint16_t extType = 0x60;
    };

    typedef senf::ConcretePacket<InternalThroughputTestPacketType> InternalThroughputTestPacket;

};

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "InternalThroughputTestPacket.cci"
//#include "InternalThroughputTestPacket.ct"
//#include "InternalThroughputTestPacket.cti"
#endif
