// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

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
