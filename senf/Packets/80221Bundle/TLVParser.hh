//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief TLVParser public header */

#ifndef HH_SENF_Packets_80221Bundle_TLVParser_
#define HH_SENF_Packets_80221Bundle_TLVParser_ 1

// Custom includes
#include "TLVParserBase.hh"
#include "MIHTypes.hh"

//#include "TLVParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

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
        SENF_PARSER_INHERIT ( MIHBaseTLVParser );
        SENF_PARSER_FIELD_RO( idLength, MIHLengthParser );
        SENF_PARSER_LABEL   ( idValue          );
        SENF_PARSER_SKIP    ( idLength(), 0    );
        SENF_PARSER_FINALIZE( MIHFIdTLVParser  );

    public:
        ///\name Value setters
        //\{
        void value( MIHFId const & id);

        void value(std::string        const & id  );
        void value(senf::MACAddress   const & addr);
        void value(senf::INet4Address const & addr);
        void value(senf::INet6Address const & addr);
        void value(senf::EUI64        const & addr);
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

        bool valueEquals(std::string        const & id  ) const;
        bool valueEquals(senf::MACAddress   const & addr) const;
        bool valueEquals(senf::INet4Address const & addr) const;
        bool valueEquals(senf::INet6Address const & addr) const;
        bool valueEquals(senf::EUI64        const & addr) const;
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
        getNAIEncodedOutputIterator(OutputIterator & i);

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

    void swap(MIHFSrcIdTLVParser src, MIHFDstIdTLVParser dst);

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
        SENF_PARSER_INHERIT ( MIHBaseTLVParser );
        SENF_PARSER_FIELD   ( value, UInt8Parser );
        SENF_PARSER_FINALIZE( MIHRegisterReqCodeTLVParser );

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
        SENF_PARSER_INHERIT ( MIHBaseTLVParser );
        SENF_PARSER_FIELD   ( value, UInt32Parser );
        SENF_PARSER_FINALIZE( MIHValidTimeIntervalTLVParser );

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
//#include "TLVParser.ct"
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
