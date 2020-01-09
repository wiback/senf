// $Id:$
//
// Copyright (C) 2008
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>


#ifndef HH_IperfPacket_
#define HH_IperfPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

///////////////////////////////hh.p////////////////////////////////////////

struct IperfUDPPacketParser : public senf::PacketParserBase
{
#   include SENF_FIXED_PARSER()

    SENF_PARSER_FIELD   ( id,      senf::Int32Parser  );
    SENF_PARSER_FIELD   ( tv_sec,  senf::UInt32Parser );
    SENF_PARSER_FIELD   ( tv_usec, senf::UInt32Parser );
    SENF_PARSER_FINALIZE( IperfUDPPacketParser        );
};

struct IperfUDPPacketType
    : public senf::PacketTypeBase,
      public senf::PacketTypeMixin<IperfUDPPacketType>
{
    typedef senf::PacketTypeMixin<IperfUDPPacketType> mixin;
    typedef senf::ConcretePacket<IperfUDPPacketType> packet;
    typedef IperfUDPPacketParser parser;

    using mixin::nextPacketRange;
    using mixin::initSize;
    using mixin::init;

    static void dump(packet p, std::ostream & os);
};

typedef senf::ConcretePacket<IperfUDPPacketType> IperfUDPPacket;


///////////////////////////////hh.e////////////////////////////////////////
#endif
