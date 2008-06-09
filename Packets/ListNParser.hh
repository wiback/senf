// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief ListNParser public header */

#ifndef HH_ListNParser_
#define HH_ListNParser_ 1

// Custom includes
#include "ListParser.hh"
#include "AuxParser.hh"

//#include "ListNParser.mpp"
#include "ListNParser.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief List parser with size-field giving number of list elements

        This parser will parse a list which size is giving by a preceding field containing the
        number of list elements. This struct is just a 'template typedef':
        \code
        senf::VectorNParser< Parser_UInt32, Parser_UInt16 >::parser MyVectorParser;
        senf::ListNParser< MyVectorParser, UInt16Parser >::parser MyListParser;
        \endcode
        This first defines a Vector of 32 bit unsigned integers with 16 bit length counter. Then it
        defines a list of such vectors with a 16 bit size field.

        \see ListParser
        \ingroup parsecollection
     */

    /** \brief Define ListNParser field

        This macro is a special helper to define a senf::ListNParser type field, a list of elements
        of type \a elt_type (a parser type) directly preceded by a numeric size field of type \a
        size_type (another parser type).

        \param[in] name field name
        \param[in] elt_type list element type
        \param[in] size_type size type
        \hideinitializer
        \ingroup packetparsermacros
     */
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_ListNParser_i_)
#define HH_ListNParser_i_
//#include "ListNParser.cci"
#include "ListNParser.ct"
#include "ListNParser.cti"
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
