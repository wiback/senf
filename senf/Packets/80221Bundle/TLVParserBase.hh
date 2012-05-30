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
    \brief TLVParserBase public header */

#ifndef HH_SENF_Packets_80221Bundle_TLVParserBase_
#define HH_SENF_Packets_80221Bundle_TLVParserBase_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

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
        void value(value_type v);

        MIHLengthParser const & operator= (value_type other);
        size_type bytes() const;
        void init() const;

        UInt8Parser length_field() const {
            return parse<UInt8Parser>( i());
        }
        FlagParser<0> extended_length_flag() const {
            return parse<FlagParser<0> >( i());
        }
        FlagParser<1> underflow_flag() const {
            return parse<FlagParser<1> >( i());
        }
        UIntFieldParser<2,8> fixed_length_field() const {
            return parse<UIntFieldParser<2,8> >( i());
        }

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

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "TLVParserBase.cci"
#include "TLVParserBase.ct"
//#include "TLVParserBase.cti"
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
