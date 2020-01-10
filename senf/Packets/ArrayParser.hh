//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ArrayParser public header */

#ifndef HH_SENF_Packets_ArrayParser_
#define HH_SENF_Packets_ArrayParser_ 1

/** \defgroup parsecollection Collection parsers

    Collection parsers are parsers which build collections from other parsers. Examples are a vector
    of 16bit unsigned integers or a list of lists of 32bit numbers and so on.

    Collection parsers provide a (reduced) STL sequence like interface. It depends on the type of
    collection parser, what kind of sequence is modelled (e.g. random access sequence, forward
    sequence etc). Most collections will also provide a kind of container wrapper to allow extensive
    manipulations of the collection contents. A container wrapper is initialized with the collection
    parser and then provides a more complete sequence interface. Additionally, the collection
    wrapper has a longer lifetime than an ordinary parser: While a parser will be invalidated
    whenever the collection is changed, the container wrapper will stay valid as long as the
    collection is changed through the wrapper (directly or indirectly, where indirectly means that a
    sub-field or sub-collection of the collection is changed). Some collections may provide even
    more lifetime guarantees but this guarantee should be met by all collection wrappers.

    \warning Parser lifetime has to be tightly checked when working with collection parsers since
    \e every change of the collections size will invalidate \e all parsers and iterators referencing
    the \e complete packet chain. Collection wrappers do \e not invalidate if the change is \e after
    the collection.

    \ingroup packetparser
*/

// Custom includes
#include "PacketParser.hh"

//#include "ArrayParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    namespace detail { template <class> class ArrayParser_iterator; }

    /** \brief Fixed size collection of fixed size elements

        ArrayParser will parse a sequence of <em>fixed size</em> parsers. The number of array
        elements is given by the \e elements template parameter and is fixed at compile time.

        Each element will be parsed by \a ElementParser, which can be any <em>fixed size</em>
        parser. The array models an STL random-access sequence with the restriction that elements
        cannot be added or removed since the size is fixed.

        \ingroup parsecollection
     */
    template <unsigned elements, class ElementParser>
    struct ArrayParser : public PacketParserBase
    {
        ArrayParser(data_iterator i, state_type s);

        static size_type const fixed_bytes = elements*ElementParser::fixed_bytes;

        void init() const;

        //-////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef ElementParser value_type;
        typedef detail::ArrayParser_iterator<value_type> iterator;
        typedef iterator const_iterator;

        static size_type size();

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;
    };

    /** \brief Define array field

        This macro is a special helper to define a senf::ArrayParser type field, a fixed size
        collection of fixed size elements.

        \param[in] name field name
        \param[in] elt_type array element type
        \param[in] size constant number of elements
        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_ARRAY(name, elt_type, size)                                                \
        typedef senf::ArrayParser<size,elt_type> BOOST_PP_CAT(name, _array_t);                    \
        SENF_PARSER_FIELD( name, BOOST_PP_CAT(name, _array_t) )

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_ArrayParser_i_)
#define HH_SENF_Packets_ArrayParser_i_
//#include "ArrayParser.cci"
#include "ArrayParser.ct"
#include "ArrayParser.cti"
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
