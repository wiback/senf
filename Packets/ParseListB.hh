// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief ParseListB public header */

#ifndef HH_ParseListB_
#define HH_ParseListB_ 1

// Custom includes
#include "ParseList.hh"

//#include "ParseListB.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    namespace detail { template <class ElementParser, class BytesParser>
                       class Parse_ListB_Policy; }

    /** \brief List parser with size-field in bytes

        This list parser will parse a list which size is given by a preceding field containing the
        length of the list in bytes. This struct is just a template typedef:
        \code
        typedef senf::Parse_VectorN< Parser_UInt32, Parser_UInt16 >::parser Parse_MyVector;
        typedef senf::Parse_ListB< Parse_MyVector, Parse_UInt16 >::parser Parse_MyList;
        \endcode
        This first defines a Vector of 32 bit unsigned integers with 16 bit length counter. Then it
        defines a list of such vectors with a 16 bit bytes field.

        \warning There are some caveats when working with this kind of list
        \li You may <b>only change the size of a contained element from a container wrapper</b>.
        \li While you hold a container wrapper, <b>only access the packet through this wrapper</b>
            or a nested wrapper either for reading or writing.

        If lists are nested, you need to allocate a container wrapper for each level and may only
        access the packet through the lowest-level active container wrapper.

        \implementation These restrictions are necessary to ensure correct recalculation of the
            <tt>bytes</tt> field. For more info, see the comments in \ref ParseListB.ih

        \see Parse_List
        \ingroup parsecollection
     */
    template <class ElementParser, class BytesParser>
    struct Parse_ListB {
        typedef Parse_List< detail::Parse_ListB_Policy<ElementParser,BytesParser> > parser;
    };

    /** \brief Define Parse_ListB field

        This macro is a special helper to define a senf::Parse_ListB type field, a list of elements
        of type \a elt_type (a parser type) directly preceded by a numeric size field of type \a
        size_type (another parser type) giving the total number of bytes of the list (not the
        element count).

        \param[in] name field name
        \param[in] elt_type list element type
        \param[in] size_type size type
        \hideinitializer
        \ingroup packetparsermacros
     */
#    define SENF_PARSER_LIST_B(name, elt_type, size_type)                                         \
        typedef senf::Parse_ListB<elt_type, size_type>::parser BOOST_PP_CAT(name, _list_t);       \
        SENF_PARSER_FIELD( name, BOOST_PP_CAT(name, _list_t) )

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_ParseListB_i_)
#define HH_ParseListB_i_
//#include "ParseListB.cci"
//#include "ParseListB.ct"
#include "ParseListB.cti"
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
