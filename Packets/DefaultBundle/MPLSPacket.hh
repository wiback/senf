// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief MPLSPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_MPLSPacket_
#define HH_SENF_Packets_DefaultBundle_MPLSPacket_ 1

// Custom includes

#include "../../Packets/Packets.hh"

//#include "MPLSPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse an MPLS packet

        Parser implementing an MPLS header.

        \image html MPLSPacket.png

        \see MPLSPacketType
     */
    struct MPLSPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( label, 20, unsigned);
        SENF_PARSER_BITFIELD( tc, 3, unsigned);
        SENF_PARSER_BITFIELD_RO( s, 1, bool);
        SENF_PARSER_FIELD( ttl, UInt8Parser);

        SENF_PARSER_FINALIZE(MPLSPacketParser);

        friend class MPLSPacketType;
    };

    /** \brief MPLS packet

        \par Packet type (typedef):
            \ref MPLSPacket

        \par Fields:
            \ref MPLSPacketParser


        \ingroup protocolbundle_default
     */
    struct MPLSPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MPLSPacketType>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<MPLSPacketType> mixin;
#endif
        typedef ConcretePacket<MPLSPacketType> packet;
        typedef MPLSPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /// Dump given MPLSPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    /** \brief MPLS packet typedef */
    typedef ConcretePacket<MPLSPacketType> MPLSPacket;


}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "MPLSPacket.cci"
//#include "MPLSPacket.ct"
//#include "MPLSPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
