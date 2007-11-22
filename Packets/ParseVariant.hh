// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer NETwork research (NET)
//     Stefan Bund <g0dil@berlios.de>
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
    \brief ParseVariant public header */

#ifndef HH_ParseVariant_
#define HH_ParseVariant_ 1

#ifndef HH_Packets_
#error "Don't include 'ParseVariant.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include "PacketParser.hh"

//#include "ParseVariant.mpp"
#include "ParseVariant.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#   ifndef SENF_LIMIT_PARSE_VARIANT
        /** \brief Maximum number of senf::Parse_Variant sub-parsers.

            This number defines the maximum number of parser arguments senf::Parse_Variant takes.
         */
#       define SENF_LIMIT_PARSE_VARIANT 6
#   endif

    /** \brief Variant parser

        This is not really a collection parser (it does not provide a collection
        interface). However, it is not a composite parser or value parser either.

        A variant parser will parse any number of sub-parsers discriminated by an arbitrary, policy
        defined condition. This is the parser to use, if the type and/or number of fields of a
        packet change depending on some condition.
        \code
        typedef senf::Parse_Variant< 
            MyVariantPolicy, 
            senf::VoidPacketParser, Parse_TypeA, Parse_TypeB> MyVariant;
        \endcode
        This typedef defines a variant parser choosing one of three sub
        parsers. senf::VoidPacketParser is an empty parser, it effectively makes this parser
        optional.

        When creating a new packet containing a variant parser, the variant parser will always be
        initialized to the first sub-parser.

        \see 
            ExampleVariantPolicy on how to implement the \a VariantPolicy \n
            \ref SENF_PARSER_VARIANT() on how to integrate the parser into another parser
        \ingroup parsecollection
     */
    template <class VariantPolicy, SENF_PARSE_VARIANT_TPL_ARGS_DFL(class P)>
    class Parse_Variant 
        : public PacketParserBase, private VariantPolicy
    {
        typedef boost::mpl::vector< SENF_PARSE_VARIANT_TPL_ARGS(P) > parsers;

    public:
        ///\name Parser interface
        ///\{

        Parse_Variant(data_iterator i, state_type s);
        Parse_Variant(VariantPolicy policy, data_iterator i, state_type s);

        size_type bytes() const;
        void init();
        
        static const size_type init_bytes = 
            senf::init_bytes<P0>::value + VariantPolicy::init_bytes;

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        unsigned variant() const;       ///< Get current variant
                                        /**< Get the currently selected variant index. The returned
                                             number directly indexes the list of sub parsers.
                                             \returns Index of currently selected variant. Integer
                                                 in the range from 0 to (number-of-sub-parsers - 1)
                                          */
        
        template <unsigned N>
        typename boost::mpl::at< parsers, boost::mpl::int_<N> >::type get() const;
                                        ///< Access sub-parser
                                        /**< This call will return the sub-parser at index \a
                                             N. This call will fail, if the currently active
                                             variant() is not \a N.
                                             \pre variant() == \a N
                                             \returns sub-parser at index \a N */

        template <unsigned N>
        void init();                    ///< Re-initialize field
                                        /**< This will reinitialize the field to the variant
                                             sub-parser at index \a N changing the currently
                                             selected variant.
                                             \post variant() == \a N */
    };

    /** \brief Variant with direct, fixed distance type field
        
        This struct is a template typedef defining a senf::Parser_Variant instantiation. It defines
        a variant parser which interprets the value returned by some other parser directly as index
        into the list of sub parsers (the numeric template argument to senf::Parse_Variant::get()
        and senf::Parser_Variant::init()).

        \code
            // Define a variant choosing between Parse_Foo and Parse_Bar depending on the directly
            // preceding 1-byte 8bit uint value
            typedef senf::Parse_Variant_Direct< senf::Parse_UInt8, 1u, 
                                                Parse_Foo, Parse_Bar >::parser MyVariant;
        \endcode

        \a ChooserType defines the type of the field used to choose the sub parser. This must be a
        fixed-size value parser. \a Distance gives the \e fixed distance of this field \e before the
        currently defined field.

        It is best to define a field of this type using \ref SENF_PARSER_VARIANT() or \ref
        SENF_PARSER_PRIVATE_VARIANT().

        \param[in] ChooserType type of chooser field (a value parser)
        \param[in] Distance    fixed distance of the chooser field before the current field
        \param[in] P           any number of sub parsers

        \see senf::Parser_Variant
        \ingroup parsecollection
     */
    template <class ChooserType, unsigned Distance, class Translator,
              SENF_PARSE_VARIANT_TPL_ARGS_DFL(class P)>
    struct Parse_Variant_Direct
    {
        typedef Parse_Variant< detail::Parse_Variant_Direct<ChooserType, Distance, Translator>,
                               SENF_PARSE_VARIANT_TPL_ARGS(P) > parser;
    };

    /** \brief Define Parse_Variant_Direct field

        This macro is a special helper to define a senf::Parse_Variant_Direct type field. This is a
        variant field which chooses the sub-type by directly taking the value of some other field.

        This is a dynamically sized parser. Nevertheless, the chooser field \e must have a \e fixed
        distance to this field, the \a chooser must be a fixed-size value parser.

        \code
        struct SomeParser : public PacketParserBase
        {
        #   include SENF_PARSER()
        
            SENF_PARSER_PRIVATE_FIELD( type, senf::Parse_UInt8 );
            SENF_PARSER_PRIVATE_VARIANT( content, type,
                                            (senf::VoidPacketParser)
                                            (senf::Parse_UInt8)
                                            (senf::Parse_UInt16)
                                            (senf::Parse_UInt24)
                                            (senf::Parse_UInt32) );

            senf::Parse_UInt8 uint8()  const { return content().get<1>(); }
            senf::Parse_UInt16 uint16() const { return content().get<2>(); }
            senf::Parse_UInt24 uint24() const { return content().get<3>(); }
            senf::Parse_UInt32 uint32() const { return content().get<4>(); }

            void disable()    const { content().init<0>(); }
            void set_uint8()  const { content().init<1>(); }
            void set_uint16() const { content().init<2>(); }
            void set_uint24)  const { content().init<3>(); }
            void set_uint23() const { content().init<4>(); }

            SENF_PARSER_FINALIZE(SomeParser);
        };
        \endcode

        The variant \c content chooses one of the sub parsers depending on the \c type field. If \c
        type is 0, senf::VoidPacketParser is selected, if it is 1, senf::Parse_UInt8 and so on. 

        It is customary, to hide the variant parser (by defining it private) and provide more
        conveniently named accessors.

        \param[in] name name of the field
        \param[in] chooser name of the field choosing the variant to use
        \param[in] types a Boost.Preprocessor style sequence of sub-parser types

        \see 
            senf::Parse_Variant \n 
            \ref SENF_PARSER_PRIVATE_VARIANT()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_VARIANT(name, chooser, types)                                              \
        SENF_PARSER_VARIANT_I(SENF_PARSER_FIELD,                                                  \
                              name,                                                               \
                              chooser,                                                            \
                              senf::detail::Parse_Variant_IdentityTranslator,                     \
                              types)

    /** \brief Define Parse_Variant_Direct field (private)
        
        \see \ref SENF_PARSER_VARIANT()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_PRIVATE_VARIANT(name, chooser, types)                                      \
        SENF_PARSER_VARIANT_I(SENF_PARSER_PRIVATE_FIELD,                                          \
                              name,                                                               \
                              chooser,                                                            \
                              senf::detail::Parse_Variant_IdentityTranslator,                     \
                              types)

    /** \brief Define Parse_Variant_Direct field with translator
        
        This is like \ref SENF_PARSER_VARIANT(), however it allows to specify a \a translator
        argument which translates between \a chooser values and type indices:
        \code
        struct SomeTranslator {
            static unsigned fromChooser(chooser_field_t::value_type value) {
                switch (value) {
                case 1  : return 0 ;
                case 5  : return 1 ;
                default : return 2 ;
                }
            }

            static chooser_field_t::value_type foChooser(unsigned value) {
                static chooser_field_t::value_type table[] const = { 1, 5, 0 };
                return table[value];
            }
        };
        \endcode
        The \a translator class must have two publicly accessible static members, \c fromChooser and
        \c toChooser. \c fromChooser takes the value as returned by the \a chooser field and must
        return the corresponding class index whereas \c toChooser takes the class index and must
        return the value to write into the \a chooser field.

        \see \ref SENF_PARSER_VARIANT()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_VARIANT_TRANS(name, chooser, translator, types)                            \
        SENF_PARSER_VARIANT_I(SENF_PARSER_FIELD,                                                  \
                              name,                                                               \
                              chooser,                                                            \
                              translator,                                                         \
                              types)

    /** \brief Define Parse_Variant_Direct field with translator (private)
        
        \see \ref SENF_PARSER_VARIANT_TRANS()
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_PRIVATE_VARIANT_TRANS(name, chooser, types)                                \
        SENF_PARSER_VARIANT_I(SENF_PARSER_PRIVATE_FIELD,                                          \
                              name,                                                               \
                              chooser,                                                            \
                              translator,                                                         \
                              types)

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_ParseVariant_i_)
#define HH_ParseVariant_i_
//#include "ParseVariant.cci"
#include "ParseVariant.ct"
#include "ParseVariant.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
