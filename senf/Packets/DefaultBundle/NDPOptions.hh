// $Id: ICMPv6TypePacket.hh 1449 2009-09-25 23:03:48Z g0dil $
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Sauer <ssauer@berlios.de>
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

#ifndef HH_SENF_Packets_DefaultBundle_NDPOptions_
#define HH_SENF_Packets_DefaultBundle_NDPOptions_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/ListOptionTypeParser.hh>

namespace senf {
    //#############################################################
    //ICMPv6 Neighbor Discovery (RFC 4861) Options
    //#############################################################
    struct NDPOptionParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type, UInt8Parser );
        SENF_PARSER_FIELD    ( length, UInt8Parser );
        SENF_PARSER_FINALIZE ( NDPOptionParser );

        typedef GenericTLVParserRegistry<NDPOptionParser> Registry;
    };
    typedef GenericTLVParserBase<NDPOptionParser> NDPGenericOptionParser;

    struct NDPSourceLLAddressTLVParser : public NDPOptionParser
    {
#      include SENF_PARSER()
        SENF_PARSER_INHERIT  ( NDPOptionParser );
        SENF_PARSER_FIELD    ( source, MACAddressParser);
        SENF_PARSER_FINALIZE ( NDPSourceLLAddressTLVParser);

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId;
            length() = 1;
        }
        static const type_t::value_type typeId = 0x01;
        void dump(std::ostream & os) const;
    };

    struct NDPTargetLLAddressTLVParser : public NDPOptionParser
    {
#      include SENF_PARSER()
        SENF_PARSER_INHERIT  ( NDPOptionParser );
        SENF_PARSER_FIELD    ( target, MACAddressParser );
        SENF_PARSER_FINALIZE ( NDPTargetLLAddressTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId;
            length() = 1;
        }
        static const UInt8Parser::value_type typeId = 0x02;
        void dump(std::ostream & os) const;
    };

    struct NDPPrefixInformationTLVParser : public NDPOptionParser
    {
#      include SENF_PARSER()
        SENF_PARSER_INHERIT          ( NDPOptionParser );
        SENF_PARSER_FIELD            ( prefixLength, UInt8Parser );
        SENF_PARSER_BITFIELD         ( l, 1, bool );
        SENF_PARSER_BITFIELD         ( a, 1, bool );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved1, 6, unsigned );
        SENF_PARSER_FIELD            ( validLifetime, UInt32Parser );
        SENF_PARSER_FIELD            ( preferredLifetime, UInt32Parser );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved2, 32, unsigned );
        SENF_PARSER_FIELD            ( prefix, INet6AddressParser );
        SENF_PARSER_FINALIZE         ( NDPPrefixInformationTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId;
            length() = 4;
            reserved1() = 0;
            reserved2() = 0;
        }
        static const UInt8Parser::value_type typeId = 0x03;
        void dump(std::ostream & os) const;
    };

    struct NDPMTUTLVParser : public NDPOptionParser
    {
#      include SENF_PARSER()
        SENF_PARSER_INHERIT          ( NDPOptionParser );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved, 16, unsigned );
        SENF_PARSER_FIELD            ( mtu, UInt32Parser );
        SENF_PARSER_FINALIZE         ( NDPMTUTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId;
            length() = 1;
            reserved() = 0;
        }
        static const UInt8Parser::value_type typeId = 0x05;
        void dump(std::ostream & os) const;
    };
}

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

