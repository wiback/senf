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
    \brief TunnelControlPacket public header */

#ifndef HH_SENF_Ext_NetEmu_P2PTunnel_TunnelCtrlPacket_
#define HH_SENF_Ext_NetEmu_P2PTunnel_TunnelCtrlPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    struct TunnelCapacityPacketParser : public PacketParserBase
    {
#        include SENF_FIXED_PARSER()
         SENF_PARSER_FIELD   ( fromClientToServer, UInt32Parser );
         SENF_PARSER_FIELD   ( fromServerToClient, UInt32Parser );
         SENF_PARSER_FINALIZE( TunnelCapacityPacketParser               );
    };

    struct TunnelCtrlPacketParser : public PacketParserBase
    {
        enum CmdCode {  // order is important.
            SetupRequest=1, SetupAck, SetupReject, TerminateRequest, TerminateAck, EchoRequest, EchoReply };

#       include SENF_PARSER()
        SENF_PARSER_FIELD   ( code,      UInt8Parser );
        SENF_PARSER_VARIANT ( code__, code,
                ( ids( na,       na,           na,           key(0,            VoidPacketParser          )) )
                ( ids( capacity, has_capacity, add_capacity, key(SetupRequest, TunnelCapacityPacketParser)) ) );
        SENF_PARSER_FINALIZE( TunnelCtrlPacketParser );
    };

    struct TunnelCtrlPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TunnelCtrlPacketType>
    {
        typedef PacketTypeMixin<TunnelCtrlPacketType> mixin;
        typedef ConcretePacket<TunnelCtrlPacketType> packet;
        typedef TunnelCtrlPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<TunnelCtrlPacketType> TunnelCtrlPacket;

}};

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::TunnelCtrlPacket );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TunnelCtrlPacket.cci"
//#include "TunnelCtrlPacket.ct"
//#include "TunnelCtrlPacket.cti"
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
