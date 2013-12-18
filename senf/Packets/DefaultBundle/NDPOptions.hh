// $Id$
//
// Copyright (C) 2010
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
//   Stefan Sauer <ssauer@berlios.de>

// Definition of non-inline non-template functions

#ifndef HH_SENF_Packets_DefaultBundle_NDPOptions_
#define HH_SENF_Packets_DefaultBundle_NDPOptions_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    //#############################################################
    //ICMPv6 Neighbor Discovery (RFC 4861) Options
    //#############################################################
    struct NDPOptionParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD( type,   UInt8Parser );
        SENF_PARSER_FIELD( length, UInt8Parser );
        SENF_PARSER_FINALIZE( NDPOptionParser  );

        typedef GenericTLVParserRegistry<NDPOptionParser> Registry;
    };

    struct NDPGenericOptionParser : public GenericTLVParserBase<NDPOptionParser>
    {
        typedef GenericTLVParserBase<NDPOptionParser> base;
        NDPGenericOptionParser(data_iterator i, state_type s) : base(i,s) {}

        senf::PacketParserBase::size_type bytes() const
        {
            return length()*8;
        }
    };

    struct NDPSourceLLAddressTLVParser : public NDPOptionParser
    {
#       include SENF_PARSER()
        SENF_PARSER_INHERIT ( NDPOptionParser             );
        SENF_PARSER_FIELD   ( source, MACAddressParser    );
        SENF_PARSER_FINALIZE( NDPSourceLLAddressTLVParser );

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
#       include SENF_PARSER()
        SENF_PARSER_INHERIT ( NDPOptionParser             );
        SENF_PARSER_FIELD   ( target, MACAddressParser    );
        SENF_PARSER_FINALIZE( NDPTargetLLAddressTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId;
            length() = 1;
        }

        senf::PacketParserBase::size_type bytes(NDPTargetLLAddressTLVParser p) {
                    return length()*8;
        }
        static const type_t::value_type typeId = 0x02;
        void dump(std::ostream & os) const;
    };

    struct NDPPrefixInformationTLVParser : public NDPOptionParser
    {
#       include SENF_PARSER()
        SENF_PARSER_INHERIT( NDPOptionParser );

        SENF_PARSER_FIELD( prefixLength, UInt8Parser );

        SENF_PARSER_BITFIELD        ( l,         1, bool     );
        SENF_PARSER_BITFIELD        ( a,         1, bool     );
        SENF_PARSER_PRIVATE_BITFIELD( reserved1, 6, unsigned );

        SENF_PARSER_FIELD( validLifetime,     UInt32Parser );
        SENF_PARSER_FIELD( preferredLifetime, UInt32Parser );

        SENF_PARSER_PRIVATE_FIELD( reserved2, UInt32Parser       );
        SENF_PARSER_FIELD        ( prefix,    INet6AddressParser );

        SENF_PARSER_FINALIZE( NDPPrefixInformationTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId;
            length() = 4;
            reserved1() = 0;
            reserved2() = 0;
        }
        senf::PacketParserBase::size_type bytes(NDPPrefixInformationTLVParser p) {
            return length()*8;
        }
        static const UInt8Parser::value_type typeId = 0x03;
        void dump(std::ostream & os) const;
    };

    struct NDPMTUTLVParser : public NDPOptionParser
    {
#       include SENF_PARSER()
        SENF_PARSER_INHERIT( NDPOptionParser );

        SENF_PARSER_PRIVATE_FIELD ( reserved, UInt16Parser );
        SENF_PARSER_FIELD         ( mtu,      UInt32Parser );

        SENF_PARSER_FINALIZE( NDPMTUTLVParser );

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "NDPOptions.cci"
//#include "NDPOptions.ct"
//#include "NDPOptions.cti"
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

