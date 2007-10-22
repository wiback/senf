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

#   ifndef SENF_LIMIT_PARSER_VARIANT
#       define SENF_LIMIT_PARSE_VARIANT 10
#   endif

#   define SENF_PARSE_VARIANT_TPL_ARGS_DFL(n)                                                     \
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( SENF_LIMIT_PARSE_VARIANT,                            \
                                             n,                                                   \
                                             boost::mpl::na )

#   define SENF_PARSE_VARIANT_TPL_ARGS(n) BOOST_PP_ENUM_PARAMS( SENF_LIMIT_PARSE_VARIANT, n )

    template <class VariantPolicy, SENF_PARSE_VARIANT_TPL_ARGS_DFL(class P)>
    class Parse_Variant 
        : public PacketParserBase, private VariantPolicy
    {
        typedef boost::mpl::vector< SENF_PARSE_VARIANT_TPL_ARGS(P) > parsers;

    public:
        Parse_Variant(data_iterator i, state_type s);
        Parse_Variant(VariantPolicy policy, data_iterator i, state_type s);

        size_type bytes() const;
        void init();
        
        static const size_type init_bytes = 
            senf::init_bytes<P0>::value + VariantPolicy::init_bytes;

        ///////////////////////////////////////////////////////////////////////////

        unsigned variant() const;
        
        template <unsigned N>
        typename boost::mpl::at< parsers, boost::mpl::int_<N> >::type get() const;

        template <unsigned N>
        void init();

    private:
    };

    template <class ChooserType, unsigned Distance, SENF_PARSE_VARIANT_TPL_ARGS_DFL(class P)>
    struct Parse_Variant_Direct
    {
        typedef Parse_Variant< detail::Parse_Variant_Direct<ChooserType, Distance>,
                               SENF_PARSE_VARIANT_TPL_ARGS(P) > parser;
    };

#   define SENF_PARSER_VARIANT_(r, data, elem) ,elem

#   define SENF_PARSER_VARIANT(name, chooser, types)                                              \
        typedef senf::Parse_Variant_Direct<                                                       \
            BOOST_PP_CAT(chooser, _t),                                                            \
            SENF_PARSER_CURRENT_FIXED_OFFSET() - SENF_PARSER_FIXED_OFFSET(chooser)                \
            BOOST_PP_SEQ_FOR_EACH( SENF_PARSER_VARIANT_, _, types )                               \
            >::parser BOOST_PP_CAT(name, _variant_t);                                             \
        SENF_PARSER_FIELD( name, BOOST_PP_CAT(name, _variant_t) )

#   define SENF_PARSER_PRIVATE_VARIANT(name, chooser, types)                                      \
        typedef senf::Parse_Variant_Direct<                                                       \
            BOOST_PP_CAT(chooser, _t),                                                            \
            SENF_PARSER_CURRENT_FIXED_OFFSET() - SENF_PARSER_FIXED_OFFSET(chooser)                \
            BOOST_PP_SEQ_FOR_EACH( SENF_PARSER_VARIANT_, _, types )                               \
            >::parser BOOST_PP_CAT(name, _variant_t);                                             \
        SENF_PARSER_PRIVATE_FIELD( name, BOOST_PP_CAT(name, _variant_t) )

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
