// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_ParseArray_
#define HH_ParseArray_ 1

/** \defgroup parsecollection Collection parsers

    Collection parsers are parsers which build collections from other parsers. Examples are a vector
    of 16bit unsigned integers or a list of lists of 32bit numbers and so on.

    Collection parsers provide a (reduced) STL sequence like interface. It depends on the type of
    collection parser, what kind of sequence is modelled (e.g. random access sequence, forward
    sequence etc). Most collections will also provide a kind of container wrapper to allow extensive
    manipulations of the collection contents. A container wrapper is initialized with the collection
    parser and then provides a more complete sequence interface.

    \important Parser lifetime has to be tightly checked when working with collection parsers since
    \e every change of the collections size will invalidate \e all parsers and iterators referencing
    the \e complete packet chain. Collection wrappers do \e not invalidate if the change is \e after
    the collection.

    \ingroup packetparser
*/

// Custom includes
#include "PacketParser.hh"

//#include "ParseArray.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    namespace detail { template <class> class Parse_Array_iterator; }

    /** \brief Fixed size collection of fixed size elements

        Parse_Array will parse a sequence of <em>fixed size</em> parsers. The number of array
        elements is given by the \e elements template parameter and is fixed at compile time. 
        
        Each element will be parsed by \a ElementParser, which can be any <em>fixed size</em>
        parser. The array models an STL random-access sequence with the restriction that elements
        cannot be added or removed since the size is fixed.

        \ingroup parsecollection
     */
    template <unsigned elements, class ElementParser>
    struct Parse_Array : public PacketParserBase
    {
        Parse_Array(data_iterator i, state_type s);

        static size_type const fixed_bytes = elements*ElementParser::fixed_bytes;

        void init() const;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef ElementParser value_type;
        typedef detail::Parse_Array_iterator<value_type> iterator;
        typedef iterator const_iterator;

        static size_type size();

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(SENF_PACKETS_DECL_ONLY) && !defined(HH_ParseArray_i_)
#define HH_ParseArray_i_
//#include "ParseArray.cci"
//#include "ParseArray.ct"
#include "ParseArray.cti"
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
