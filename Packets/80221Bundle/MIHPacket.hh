// $Id$
//
// Copyright (C) 2009
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
    \brief MIH protocol public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHPacket_
#define HH_SENF_Packets_80221Bundle_MIHPacket_ 1

// Custom includes
#include "../../Packets/Packets.hh"
#include "../../Socket/Protocols/Raw/MACAddress.hh"
#include "../../Socket/Protocols/INet/INet4Address.hh"
#include "../../Socket/Protocols/INet/INet6Address.hh"
#include "TLVPacket.hh"
#include <boost/function_output_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include "boost/variant.hpp"


//#include "MIHPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct MIHMessageRegistry {
        // MIH messages registry
        typedef boost::uint16_t key_t;
    };

#   define SENF_MIH_PACKET_REGISTRY_REGISTER( packet )                    \
        SENF_PACKET_REGISTRY_REGISTER(                                    \
            senf::MIHMessageRegistry, packet::type::MESSAGE_ID, packet )
    
    class MIHFId 
        : public boost::variant< boost::blank, senf::MACAddress, senf::INet4Address, 
                senf::INet6Address, std::string, senf::EUI64 >,
          public boost::less_than_comparable<MIHFId>,
          public boost::equality_comparable<MIHFId>
    {
    public:
        enum Type { Empty, MACAddress, INet4Address, INet6Address, String, EUI64 };
      
        MIHFId();
        MIHFId(senf::MACAddress const & addr);
        MIHFId(senf::INet4Address const & addr);
        MIHFId(senf::INet6Address const & addr);
        MIHFId(std::string const & addr);
        MIHFId(senf::EUI64 const & addr);
        
        Type type() const;
        bool operator==(MIHFId const & other) const;
        bool operator<(MIHFId const & other) const; 
        
    private:
        struct GetTypeVisitor : public boost::static_visitor<Type> {
            Type operator()(boost::blank const &) const { return Empty; }
            Type operator()(senf::MACAddress const &) const { return MACAddress; }
            Type operator()(senf::INet4Address const &) const { return INet4Address; }
            Type operator()(senf::INet6Address const &) const { return INet6Address; }
            Type operator()(std::string const & ) const { return String; }
            Type operator()(senf::EUI64 const &) const { return EUI64; }
        };
        struct EqualsVisitor : public boost::static_visitor<bool> {
            template <typename T, typename U>
            bool operator()(T const &, U const &) const {
                return false;
            }
            template <typename T>
            bool operator()( const T & lhs, const T & rhs ) const {
                return lhs == rhs;
            }
        };
        struct LessThanVisitor : public boost::static_visitor<bool> {
            template <typename T, typename U>
            bool operator()(T const &, U const &) const {
                return false;
            }
            template <typename T>
            bool operator()( const T & lhs, const T & rhs ) const {
                return lhs < rhs;
            }
        };
    };
    
    
    /** \brief Parse a MIHF_ID

         the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
         we could set maxLengthValue in init(), but for the most MIHF_IDs the default
         maximum length of 127 should be enough.
         
         \note you must call mihfIdPacket.maxLengthValue( 253) *before*
         setting longer MIHF_IDs values.
    */
    class MIHFId_TLVParser : public BaseTLVPacketParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( BaseTLVPacketParser );
        SENF_PARSER_SKIP     ( length(), 0         );
        SENF_PARSER_FINALIZE ( MIHFId_TLVParser    );

        std::string asString() const;
        void setString(std::string const &id);

        senf::MACAddress asMACAddress() const;
        void setMACAddress(senf::MACAddress const &mac);

        senf::INet4Address asINet4Address() const;
        void setINet4Address(senf::INet4Address const &addr);

        senf::INet6Address asINet6Address() const;
        void setINet6Address(senf::INet6Address const &addr);
        
        senf::EUI64 asEUI64() const;
        void setEUI64(senf::EUI64 const &addr);

        MIHFId valueAs(MIHFId::Type type) const;
        
    private:
        template <class OutputIterator>
        struct binaryNAIEncoder {
            binaryNAIEncoder(OutputIterator &i) : i_(i) {}
            void operator()(const boost::uint8_t &v) const {
                *i_++ = '\\';
                *i_++ = v;
            }
            OutputIterator &i_;
        };
        template <class OutputIterator>
        static boost::function_output_iterator<binaryNAIEncoder<OutputIterator> > getNAIEncodedOutputIterator(OutputIterator i) {
            return boost::make_function_output_iterator(binaryNAIEncoder<OutputIterator>(i));
        }

        struct binaryNAIDecoder {
            binaryNAIDecoder() : readNextByte_(true) {}
            bool operator()(const boost::uint8_t &v) {
                readNextByte_ = readNextByte_ ? false : true;
                return readNextByte_;
            }
            bool readNextByte_;
        };
        template <class Iterator>
        static boost::filter_iterator<binaryNAIDecoder, Iterator> getNAIDecodedIterator(Iterator begin, Iterator end) {
            return boost::make_filter_iterator<binaryNAIDecoder>(begin, end);
        }
    };
 
    /** \brief Parse a MIH packet

        Parser implementing the MIH header. The fields implemented are:
        \image html MIHPacket.png

        \see MIHPacketType
     */
    struct MIHPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()

        SENF_PARSER_BITFIELD_RO ( version,       4,  unsigned );
        SENF_PARSER_BITFIELD    ( ackRequest,    1,  bool     );
        SENF_PARSER_BITFIELD    ( ackResponse,   1,  bool     );
        SENF_PARSER_BITFIELD    ( uir,           1,  bool     );
        SENF_PARSER_BITFIELD    ( moreFragment,  1,  bool     );
        SENF_PARSER_BITFIELD    ( fragmentNr,    7,  unsigned );
        SENF_PARSER_SKIP_BITS   ( 1                           );

        // MIH message ID (MID)
        SENF_PARSER_FIELD    ( messageId, UInt16Parser ); //<pkgdraw:hide
        SENF_PARSER_GOTO     ( messageId               );
        SENF_PARSER_BITFIELD ( sid,     4,  unsigned   );
        SENF_PARSER_BITFIELD ( opcode,  2,  unsigned   );
        SENF_PARSER_BITFIELD ( aid,    10,  unsigned   );
        
        SENF_PARSER_SKIP_BITS ( 4                           );
        SENF_PARSER_BITFIELD  ( transactionId, 12, unsigned );
        SENF_PARSER_FIELD_RO  ( payloadLength, UInt16Parser );
        
        SENF_PARSER_GOTO_OFFSET( 8, 8); // just to limit the offset calculation
        
        // Source MIHF Id
        SENF_PARSER_FIELD ( src_mihfId, MIHFId_TLVParser );
        // Destination MIHF Id
        SENF_PARSER_FIELD ( dst_mihfId, MIHFId_TLVParser );

        SENF_PARSER_FINALIZE ( MIHPacketParser );

        SENF_PARSER_INIT() {
            version_() = 1;
            src_mihfId().type() = 1;
            dst_mihfId().type() = 2;
        }

        friend class MIHPacketType;
    };

    /** \brief MIH packet

        \par Packet type (typedef):
            \ref MIHPacket

        \par Fields:
            \ref MIHPacketParser

        \ingroup protocolbundle_80221
     */
    struct MIHPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MIHPacketType, MIHMessageRegistry>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<MIHPacketType, MIHMessageRegistry> mixin;
#endif
        typedef ConcretePacket<MIHPacketType> packet; ///< MIH packet typedef
        typedef MIHPacketParser parser;               ///< typedef to the parser of MIH packet

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        /** \brief Dump given MIH packet in readable form to given output stream */
        static void dump(packet p, std::ostream &os);
        static void finalize(packet p);
        static factory_t nextPacketType(packet p);
        
        enum ResponseStatus { Success, UnspecifiedFailure, Rejected, AuthorizationFailure, NetworkError };
    };

    /** \brief MIH packet typedef */
    typedef ConcretePacket<MIHPacketType> MIHPacket;


    struct MIHPayloadPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_LIST ( tlv_list, packetSize(), GenericTLVPacketParser );

        SENF_PARSER_FINALIZE ( MIHPayloadPacketParser );
    };

    struct MIHPayloadPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<MIHPayloadPacketType>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<MIHPayloadPacketType> mixin;
#endif
        typedef ConcretePacket<MIHPayloadPacketType> packet; ///< MIH Payload packet typedef
        typedef MIHPayloadPacketParser parser; ///< typedef to the parser of MIH Payload packet

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        /** \brief Dump given MIHPayload in readable form to given output stream */
        static void dump(packet p, std::ostream &os);
    };

     /** \brief MIH Payload packet typedef */
    typedef ConcretePacket<MIHPayloadPacketType> MIHPayloadPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#include "MIHPacket.cci"
//#include "MIHPacket.ct"
//#include "MIHPacket.cti"
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
