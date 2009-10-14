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
    \brief TLVParser public header */

#ifndef HH_SENF_Packets_80221Bundle_TLVParser_
#define HH_SENF_Packets_80221Bundle_TLVParser_ 1

// Custom includes
#include <algorithm>
#include <senf/Packets/Packets.hh>
#include "MIHTypes.hh"

//#include "TLVParser.mpp"
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
        void resize_(size_type size);
    };  
        

    /** \brief Base class for MIH TLV parsers
     
         MIHBaseTLVParser is the abstract base class for MIH TLV parsers. It defines the
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
         \endcode
         
         You have to adjust the maximum length value with the \ref maxLengthValue function 
         before the length value is set. The default maximum value is 127. So, in the above
         example adding more than 21 MACAddresses to the vector will throw a TLVLengthException
         if you don't call \c macAddressesTLVPacket->maxLengthValue( \e some_value) before.
         
         \see MIHTLVLengthParser \n
           MIHGenericTLVParser \n
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
            protect(), length_().maxValue(v);
        }
        
        /** \brief shrink size of length field to minimum
    
            The size of the length field will be decreased to minimum necessary to hold
            the current length value.
         */
        void finalizeLength() { 
            protect(), length_().finalize(); 
        };
        
    protected:
        /// resize the packet after the length field to given size
        senf::safe_data_iterator resizeValueField(MIHTLVLengthParser::value_type size);
    };

        
    /** \brief Parser for a generic TLV packet
     */
    struct MIHGenericTLVParser
        : public GenericTLVParserBase<MIHBaseTLVParser>
    {
        typedef senf::GenericTLVParserBase<MIHBaseTLVParser> base;
        MIHGenericTLVParser(data_iterator i, state_type s) : base(i,s) {}

        void init() const {
            defaultInit();
            maxLengthValue( MIHTLVLengthParser::max_value);
        }
        
        using base::init;
    };
        
    /** \brief Parse a MIHF_ID

         the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
         we could set maxLengthValue in init(), but for the most MIHF_IDs the default
         maximum length of 127 should be enough.
         
         \note you must call mihfIdPacket.maxLengthValue( 253) *before*
         setting longer MIHF_IDs values.
    */
    class MIHFId_TLVParser : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( MIHBaseTLVParser );
        SENF_PARSER_SKIP     ( length(), 0      );
        SENF_PARSER_FINALIZE ( MIHFId_TLVParser );
        
    public:
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


}


///////////////////////////////hh.e////////////////////////////////////////
#include "TLVParser.cci"
//#include "TLVParser.ct"
//#include "TLVParser.cti"
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
