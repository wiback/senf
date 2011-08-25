// $Id$
//
// Copyright (C) 2007
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief TLVParser public header */

#ifndef HH_SENF_Packets_80221Bundle_TLVParser_
#define HH_SENF_Packets_80221Bundle_TLVParser_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "MIHTypes.hh"

//#include "TLVParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class MIHLengthParser
        : public detail::packet::IntParserOps<MIHLengthParser, boost::uint32_t>,
          public PacketParserBase
    {
    public:
        MIHLengthParser(data_iterator i, state_type s) : PacketParserBase(i,s) {}

        typedef boost::uint32_t value_type;
        static const size_type init_bytes = 1;
        static value_type const min_value = 0;
        static value_type const max_value = 4294967295u;

        value_type value() const;
        void value(value_type const & v);

        MIHLengthParser const & operator= (value_type other);
        size_type bytes() const;
        void init() const;

#       include SENF_PARSER()
        SENF_PARSER_PRIVATE_FIELD ( length_field, UInt8Parser );
        SENF_PARSER_GOTO( length_field );
        SENF_PARSER_PRIVATE_BITFIELD ( extended_length_flag, 1,  bool     );
        SENF_PARSER_PRIVATE_BITFIELD ( underflow_flag,       1,  bool     );
        SENF_PARSER_PRIVATE_BITFIELD ( fixed_length_field,   6,  unsigned );

        void finalize();
        void capacity(value_type v);
        value_type capacity() const;

    private:
        void resize_(size_type size);
    };


    /** \brief Base class for MIH TLV parsers

         MIHBaseTLVParser is the abstract base class for MIH TLV parsers. It defines the
         \ref type() field as an \ref senf::UInt8Parser and the \ref length() field as a
         MIHLengthParser. The length field is read-only.

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

         You have to adjust the maximum length value with the \ref maxLength function
         before the length value is set. The default maximum value is 128. So, in the above
         example adding more than 21 MACAddresses to the vector will throw a TLVLengthException
         if you don't call \c maxLength( \e some_value) before.

         \see MIHLengthParser \n
           MIHGenericTLVParser \n
     */
    class MIHBaseTLVParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type,   UInt8Parser        );
        SENF_PARSER_FIELD_RO ( length, MIHLengthParser );
        SENF_PARSER_FINALIZE ( MIHBaseTLVParser           );

        /** \brief shrink size of the TLV length field to minimum

            The size of the length field will be decreased to minimum necessary to hold
            the current length value.
         */
        void finalize();

        typedef GenericTLVParserRegistry<MIHBaseTLVParser> Registry;

    protected:
        /** \brief set maximum value of TLV length field

            The size of the length field will be increased if necessary.
            \param v maximum value of length field
         */
        void maxLength(MIHLengthParser::value_type maxl) const;

        void validateType(boost::uint8_t type) const;
        void validateTypeLength(boost::uint8_t type, MIHLengthParser::value_type length) const;
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
            maxLength( MIHLengthParser::max_value);
        }

        using base::init;
        using base::maxLength;
    };


    /** \brief Base class for list TLV parser
     */
    struct MIHBaseListTLVParser
        : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( MIHBaseTLVParser );
        SENF_PARSER_FIELD_RO ( listSize, MIHLengthParser );
        SENF_PARSER_FINALIZE ( MIHBaseListTLVParser );

        void maxListSize(MIHLengthParser::value_type maxl = MIHLengthParser::max_value) const;
    };

    template <class Self>
    struct MIHListTLVParserMixin
    {
        void finalize();
    };


    /** \brief Parse a MIHF_ID

         Note that the maximum length of a MIHF_ID is 253 octets (see F.3.11 in 802.21)
         We could set maxLength in init(), but for the most MIHF_IDs the default
         maximum length of 128 should be enough.

         \note you must call maxIdLength( 253) *before* setting MIHF_IDs values longer
             than 128.

         \see MIHFId
    */
    class MIHFIdTLVParser : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( MIHBaseTLVParser );
        SENF_PARSER_FIELD_RO ( idLength, MIHLengthParser );
        SENF_PARSER_LABEL    ( idValue          );
        SENF_PARSER_SKIP     ( idLength(), 0    );
        SENF_PARSER_FINALIZE ( MIHFIdTLVParser  );

    public:
        ///\name Value setters
        //\{
        void value( MIHFId const & id);

        void value( std::string        const & id  );
        void value( senf::MACAddress   const & addr);
        void value( senf::INet4Address const & addr);
        void value( senf::INet6Address const & addr);
        void value( senf::EUI64        const & addr);
        //\}

        ///\name Value getters
        //\{
        MIHFId valueAs( MIHFId::Type type) const;

        std::string        valueAsString()       const;
        senf::MACAddress   valueAsMACAddress()   const;
        senf::INet4Address valueAsINet4Address() const;
        senf::INet6Address valueAsINet6Address() const;
        senf::EUI64        valueAsEUI64()        const;
        //\}

        ///\name Value comparisons
        //\{
        bool valueEquals( MIHFId const & id) const;

        bool valueEquals( std::string        const & id  ) const;
        bool valueEquals( senf::MACAddress   const & addr) const;
        bool valueEquals( senf::INet4Address const & addr) const;
        bool valueEquals( senf::INet6Address const & addr) const;
        bool valueEquals( senf::EUI64        const & addr) const;
        //\}

        void dump(std::ostream & os) const;
        void maxIdLength(boost::uint8_t maxl) const;
        void finalize();

    private:
        /// resize the packet after the length field to given size
        senf::safe_data_iterator resizeValueField(MIHLengthParser::value_type size);

        data_iterator valueBegin() const;
        data_iterator valueEnd() const;

        template <class OutputIterator>
        struct binaryNAIEncoder {
            binaryNAIEncoder(OutputIterator & i);
            void operator()(boost::uint8_t v);
            OutputIterator & i_;
        };

        template <class OutputIterator>
        static boost::function_output_iterator<binaryNAIEncoder<OutputIterator> >
        getNAIEncodedOutputIterator(OutputIterator i);

        struct binaryNAIDecoder {
            binaryNAIDecoder();
            bool operator()(boost::uint8_t v);
            bool readNextByte_;
        };

        template <class Iterator>
        static boost::filter_iterator<binaryNAIDecoder, Iterator>
        getNAIDecodedIterator(Iterator begin, Iterator end);

        struct ValueSetterVisitor : public boost::static_visitor<> {
            MIHFIdTLVParser & parser;
            ValueSetterVisitor( MIHFIdTLVParser & p) : parser(p) {}
            void operator()( boost::blank ) const {
                parser.value( std::string());
            }
            template <typename MIHFIdType>
            void operator()( MIHFIdType const & id ) const {
                parser.value( id);
            }
        };

        struct ValueEqualsVisitor : public boost::static_visitor<bool> {
            MIHFIdTLVParser const & parser;
            ValueEqualsVisitor( MIHFIdTLVParser const & p) : parser(p) {}
            bool operator()( boost::blank ) const {
                return parser.idLength() == 0;
            }
            template <typename MIHFIdType>
            bool operator()( MIHFIdType const & id ) const {
                return parser.valueEquals( id);
            }
        };
    };

    /** \brief Parser for 802.21 source MIHF_ID TLV
     */
    struct MIHFSrcIdTLVParser : public MIHFIdTLVParser
    {
        MIHFSrcIdTLVParser(data_iterator i, state_type s) : MIHFIdTLVParser(i,s) {}

        void init() const {
            defaultInit();
            type() << typeId+0;
        }
        static type_t::value_type const typeId = 1;
        void dump(std::ostream & os) const;
        void validate() const;
    };

    /** \brief Parser for 802.21 destination MIHF_ID TLV
     */
    struct MIHFDstIdTLVParser : public MIHFIdTLVParser
    {
        MIHFDstIdTLVParser(data_iterator i, state_type s) : MIHFIdTLVParser(i,s) {}

        void init() const {
            defaultInit();
            type() << typeId+0;
        }
        static type_t::value_type const typeId = 2;
        void dump(std::ostream & os) const;
        void validate() const;
    };

    /** \brief Parser for 802.21 Status TLV
     */
    struct MIHStatusTLVParser : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT ( MIHBaseTLVParser   );
        SENF_PARSER_FIELD   ( value, UInt8Parser );
        SENF_PARSER_FINALIZE( MIHStatusTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() << typeId+0;
            length_() = 1;
        }
        static type_t::value_type const typeId = 3;
        void dump(std::ostream & os) const; ///< dump string representation to given stream
        void validate() const;

        enum StatusCode {
            Success, UnspecifiedFailure, Rejected, AuthorizationFailure, NetworkError };
    };

    struct MIHRegisterReqCodeTLVParser : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( MIHBaseTLVParser );
        SENF_PARSER_FIELD    ( value, UInt8Parser );
        SENF_PARSER_FINALIZE ( MIHRegisterReqCodeTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId+0;
            length_() = 1;
        }
        static type_t::value_type const typeId = 11;
        void dump(std::ostream & os) const; ///< dump string representation to given stream
        void validate() const;

        enum RequestCode { Registration, ReRegistration };
    };

    struct MIHValidTimeIntervalTLVParser : public MIHBaseTLVParser
    {
    #   include SENF_PARSER()
        SENF_PARSER_INHERIT  ( MIHBaseTLVParser );
        SENF_PARSER_FIELD    ( value, UInt32Parser );
        SENF_PARSER_FINALIZE ( MIHValidTimeIntervalTLVParser );

        SENF_PARSER_INIT() {
            defaultInit();
            type() = typeId+0;
            length_() = 4;
        }
        static type_t::value_type const typeId = 12;
        void dump(std::ostream & os) const; ///< dump string representation to given stream
        void validate() const;
    };

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "TLVParser.cci"
#include "TLVParser.ct"
#include "TLVParser.cti"
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
