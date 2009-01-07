// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief TLVPacket public header */

#ifndef HH_SENF_Packets_80221Bundle_TLVPacket_
#define HH_SENF_Packets_80221Bundle_TLVPacket_ 1

// Custom includes
#include <algorithm>
#include "../../Packets/Packets.hh"

//#include "TLVPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief xxx
        
        \todo document me
        \todo add usefull exceptions strings
         
        \ingroup protocolbundle_80221
    */
    struct UnsuportedTLVPacketException : public senf::Exception
    { UnsuportedTLVPacketException() 
          : senf::Exception("length of length can be max. 4 bytes. Sorry."){} };

    /** \brief xxx
        \todo document me
        
        \ingroup protocolbundle_80221
    */
    class DynamicTLVLengthParser 
        : public detail::packet::IntParserOps<DynamicTLVLengthParser, boost::uint32_t>,
          public PacketParserBase
    {
    public:
        DynamicTLVLengthParser(data_iterator i, state_type s) : PacketParserBase(i,s) {}

        typedef boost::uint32_t value_type;
        static const size_type init_bytes = 1;
        static value_type const min_value = 0;
        static value_type const max_value = 4294967295u;

        value_type value() const;
        void value(value_type const & v);
        
        DynamicTLVLengthParser const & operator= (value_type other);
        size_type bytes() const;
        void init() const;

#       include SENF_PARSER()
        
        SENF_PARSER_PRIVATE_BITFIELD ( extended_length_flag, 1,  bool     );
        SENF_PARSER_PRIVATE_BITFIELD ( fixed_length_field,   7,  unsigned );
        
    private:

//        typedef FlagParser      <    0 > ExtendedLengthFlagParser;
//        typedef UIntFieldParser < 1, 8 > FixedLengthParser;
//
//        ExtendedLengthFlagParser extended_length_flag() const {
//            return parse<ExtendedLengthFlagParser>( 0 );
//        }
//
//        FixedLengthParser fixed_length_field() const {
//            return parse<FixedLengthParser>( 0 );
//        }
        
        void resize(size_type size, SafePacketParserWrapper<DynamicTLVLengthParser> &safeThis);
    };  
        
    /** \brief parse TLVPacket Packet
    
        \todo document me
     
        \see TLVPacketType
        
        \ingroup protocolbundle_80221
     */
    struct GenericTLVPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()
        
        SENF_PARSER_FIELD    ( type,   UInt8Parser                );
        SENF_PARSER_FIELD_RO ( length, DynamicTLVLengthParser     );
        SENF_PARSER_VECTOR   ( value,  bytes(length), UInt8Parser );
        
        SENF_PARSER_FINALIZE( GenericTLVPacketParser );
    };
    
    /** \brief generic TLV Packet type
        
        \todo document me
        
        \ingroup protocolbundle_80221
     */
    struct GenericTLVPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<GenericTLVPacketType>
    {
        typedef PacketTypeMixin<GenericTLVPacketType> mixin;
        typedef ConcretePacket<GenericTLVPacketType> packet;
        typedef GenericTLVPacketParser parser;

//        static optional_range nextPacketRange(packet p);
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
//        static void finalize(packet p);
        static void dump(packet p, std::ostream & os);
    };
    
    typedef ConcretePacket<GenericTLVPacketType> GenericTLVPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TLVPacket.cci"
//#include "TLVPacket.ct"
//#include "TLVPacket.cti"
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
