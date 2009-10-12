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
#include <senf/Packets/Packets.hh>

//#include "TLVPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct MIHTLVLengthException : public senf::Exception
    { 
        MIHTLVLengthException() 
          : senf::Exception("MIHTLVLengthException") {} 
    };

    
    class MIHTLVLengthParser 
        : public detail::packet::IntParserOps<MIHTLVLengthParser, boost::uint32_t>,
          public PacketParserBase
    {
    public:
        MIHTLVLengthParser(data_iterator i, state_type s) : PacketParserBase(i,s) {}

        typedef boost::uint32_t value_type;
        static const size_type init_bytes = 1;
        static value_type const min_value = 0;
        static value_type const max_value = 4294967295u;

        value_type value() const;
        void value(value_type const & v);
        
        MIHTLVLengthParser const & operator= (value_type other);
        size_type bytes() const;
        void init() const;

#       include SENF_PARSER()
        SENF_PARSER_PRIVATE_FIELD ( length_field, UInt8Parser );
        SENF_PARSER_GOTO( length_field );
        SENF_PARSER_PRIVATE_BITFIELD ( extended_length_flag, 1,  bool     );
        SENF_PARSER_PRIVATE_BITFIELD ( underflow_flag,       1,  bool     );
        SENF_PARSER_PRIVATE_BITFIELD ( fixed_length_field,   6,  unsigned );

        void finalize();
        void maxValue(value_type v);
        value_type maxValue() const;
    private:
        void resize(size_type size);
    };  
        

    /** \brief Base class for TLV-Packet-Parsers
     
         MIHBaseTLVParser is the abstract base class for TLV-Packet-Parsers. It defines the
         \ref type() field as an \ref senf::UInt8Parser and the \ref length() field as a 
         MIHTLVLengthParser. The length field is read-only. 
         
         To create your own \c TLVParser you have to inherit from MIHBaseTLVParser (don't 
         forget \ref SENF_PARSER_INHERIT) and define the \c value field. In the following example 
         the value is a vector of MacAddresses: 
         \code
         struct MacAddressesTLVParser : public MIHBaseTLVParser {
         #   include SENF_PARSER()        
             SENF_PARSER_INHERIT ( MIHBaseTLVParser );
             SENF_PARSER_VECTOR  ( value, bytes(length), senf::MACAddressParser );
             SENF_PARSER_FINALIZE( MacAddressesTLVParser );
         };
         
         struct MacAddressesTLVPacketType : public PacketTypeBase {
            typedef MacAddressesTLVParser parser;
            ...
            static void finalize(ConcretePacket<MacAddressesTLVPacketType> p) { 
                p->finalizeLength();
            }
         };
         \endcode
         
         You have to adjust the maximum length value with the \ref maxLengthValue function 
         before the length value is set. The default maximum value is 127. So, in the above
         example adding more than 21 MACAddresses to the vector will throw a TLVLengthException
         if you don't call \c macAddressesTLVPacket->maxLengthValue( \e some_value) before.
         
         \see MIHTLVLengthParser \n
           MIHGenericTLVPacketParser \n
     */
    class MIHBaseTLVParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type,   UInt8Parser        );
        SENF_PARSER_FIELD_RO ( length, MIHTLVLengthParser );
        SENF_PARSER_FINALIZE ( MIHBaseTLVParser           );
        
        /** \brief set maximum value of length field
    
            The size of the length field will be increased if necessary.
            \param v maximum value of length field
         */
        void maxLengthValue(MIHTLVLengthParser::value_type v) const {
            length_().maxValue(v);
        }
        
        /** \brief shrink size of length field to minimum
    
            The size of the length field will be decreased to minimum necessary to hold
            the current length value.
         */
        void finalizeLength() { 
            length_().finalize(); 
        };
        
    protected:
        /// resize the packet after the length field to given size
        senf::safe_data_iterator resizeValueField(MIHTLVLengthParser::value_type size);
    };

        
    /** \brief Parser for a generic TLV packet

        \see MIHGenericTLVPacketType
     */
    struct MIHGenericTLVPacketParser 
        : public GenericTLVParserBase<MIHBaseTLVParser>
    {
        typedef senf::GenericTLVParserBase<MIHBaseTLVParser> base;
        MIHGenericTLVPacketParser(data_iterator i, state_type s) : base(i,s) {}

        void init() const {
            defaultInit();
            maxLengthValue( MIHTLVLengthParser::max_value);
        }        
    };
    
    /** \brief Generic TLV packet

        \par Packet type (typedef):
            \ref MIHGenericTLVPacket

        \image html TLV.png
        
        \ingroup protocolbundle_80221
     */
    struct MIHGenericTLVPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MIHGenericTLVPacketType>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<MIHGenericTLVPacketType> mixin;
#endif
        typedef ConcretePacket<MIHGenericTLVPacketType> packet; ///< GenericTLV packet typedef
        typedef MIHGenericTLVPacketParser parser;               ///< typedef to the parser of GenericTLV packet

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
        /** \brief Dump given MIHGenericTLVPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);  
        static void finalize(packet p);  ///< Finalize packet.
                                         /**< shrink size of length field to minimum 
                                              \see MIHBaseTLVParser::finalizeLength() */
    };
    
    /** \brief GenericTLV packet typedef
        \ingroup protocolbundle_80221
     */
    typedef ConcretePacket<MIHGenericTLVPacketType> MIHGenericTLVPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#include "TLVPacket.cci"
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
