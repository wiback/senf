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

// Custom includes
#include <utility> // for std::pair
#include "ParserBase.hh"

//#include "ParseArray.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    namespace impl { template <class,class> class Parse_Array_iterator; }

    /* Parse_Array has the external interface of a container class
     */
    template <unsigned elements, class Parser, class Iterator=nil, class IPacket=nil>
    struct Parse_Array : public ParserBase<Iterator,IPacket>
    {
        ///////////////////////////////////////////////////////////////////////////
        // Parser interface

        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Array<elements,Parser,I,P> parser; };
        typedef Iterator byte_iterator;

        Parse_Array();
        explicit Parse_Array(Iterator const & i);

        static unsigned bytes();
        bool check(Iterator const & e) const;
        void init() const;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename Parser::template rebind<Iterator>::parser value_type;
        typedef impl::Parse_Array_iterator<value_type,Iterator> iterator;
        typedef unsigned size_type;
        typedef int difference_type;
        typedef std::pair<iterator,iterator> range_type;

        static size_type size();

        iterator begin() const;
        iterator end() const;
        range_type range() const;
        iterator value() const;

        value_type operator[](difference_type i) const;

        template <class InputIterator>
        Parse_Array const & operator= (InputIterator const & i);
    };

}

///////////////////////////////hh.e////////////////////////////////////////
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
