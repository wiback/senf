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

#ifndef HH_ParseVec_
#define HH_ParseVec_ 1

// Custom includes
#include <utility> // for std::pair
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp> // for boost::noncopyable
#include "ParserBase.hh"
#include "ParseArray.hh" // for Parse_Array_iterator

//#include "ParseVec.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    template <class Parser, class SizeParser, class Container> class Parse_Vector_wrapper;

    template <class Parser, class SizeParser, class Iterator=nil, class IPacket=nil>
    struct Parse_Vector : public ParserBase<Iterator,IPacket>
    {
        typedef typename SizeParser::template rebind<Iterator>::parser size_parser;

        ///////////////////////////////////////////////////////////////////////////
        // Parser interface

        template <class I=nil, class P=nil>
        struct rebind { typedef Parse_Vector<Parser,SizeParser,I,P> parser; };
        typedef Iterator byte_iterator;

        explicit Parse_Vector(SizeParser const & size);
        Parse_Vector(size_parser const & size, Iterator const & i);

        unsigned bytes() const;
        void check(Iterator const & e) const;
        void init() const;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename Parser::template rebind<Iterator>::parser value_type;
        typedef impl::Parse_Array_iterator<value_type,Iterator> iterator;
        typedef unsigned size_type;
        typedef int difference_type;
        typedef std::pair<iterator,iterator> range_type;

        template <class Container>
        struct wrapper { typedef Parse_Vector_wrapper<value_type, size_parser, Container> t; };

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;
        range_type range() const;
        range_type value() const;

        value_type operator[](difference_type i) const;

     private:
        size_parser size_;

        template <class P, class SP, class C> friend class Parse_Vector_wrapper;
    };

    /** \brief

        Holds a reference to the container !
      */
    template <class Parser, class SizeParser, class Container>
    class Parse_Vector_wrapper
        : public boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef Container container;
        typedef SizeParser size_parser;
        typedef typename Parser::byte_iterator byte_iterator;
        typedef Parser value_type;
        typedef impl::Parse_Array_iterator<value_type,byte_iterator> iterator;
        typedef unsigned size_type;
        typedef int difference_type;
        typedef std::pair<iterator,iterator> range_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        template <class P, class SP, class I, class IP>
        Parse_Vector_wrapper(Parse_Vector<P,SP,I,IP> const & vector, Container & container);

        // no default constructor
        // no copy
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name APacketRegistry.essors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;
        range_type range() const;

        value_type operator[](difference_type i) const;

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Mutators
        ///@{

        void shift(iterator pos, size_type n=1);
        template <class Value>
        void insert(iterator pos, Value const & t);
        template <class Value>
        void insert(iterator pos, size_type n, Value const & t);
        template <class InputIterator>
        void insert(iterator pos, InputIterator f, InputIterator l);

        void erase(iterator pos, size_type n=1);
        void erase(iterator f, iterator l);
        void clear();

        ///@}

    protected:

    private:

        size_type i_;
        size_type size_i_;
        Container & container_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParseVec.cci"
#include "ParseVec.ct"
#include "ParseVec.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
