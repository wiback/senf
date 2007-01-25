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

/** \file

    \idea Add an optional state to the sentinel and an optional
    transition function. See ParseListS.hh for more.

    We should write a baseclass for sentinels which has no \c check()
    member, en empty \c next() member and \c void as the state
    type. This simplifies writing simple sentinels.

    The parse_listS iterator will have to pass the state in addition
    to the current list element to \c check(). The \c next() member
    will be invoked to advance the iterator. It is passer the current
    element and a (non-const) reference to the state which it may
    update. The Parse_ListS constructor must take an arbitrary number
    of additional arguments which are forwarded to the state
    initialization. 
    
    This structure makes it simple to optimize away the overhead if
    the state type is void. If we would always instantiate the
    sentinel, this will always take up space.

    Another possibility would be to always instantiate the sentinel
    and make the baseclass mandatory. The baseclass would then hold
    the current raw iterator. The iterator itself would ONLY include a
    single sentinel instance .. I think, this is the best solution,
    sentinel members then have intrinsic access to the
    state. Arguments are forwarded from the list constructor to the
    Sentinel constructor.
 */

#ifndef HH_ParseListS_
#define HH_ParseListS_ 1

// Custom includes
#include <utility> // for std::pair
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp> // for boost::noncopyable
#include "ParserBase.hh"

//#include "ParseListS.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    template <class Parser, class Sentinel, class Container> class Parse_ListS_wrapper;
    namespace impl { 
        template <class Parser, class Sentinel, class Container> class Parse_ListS_iterator; 
    }

    template <class Parser, class Sentinel, class Iterator=nil, class IPacket=nil>
    struct Parse_ListS : public ParserBase<Iterator,IPacket>
    {
        ///////////////////////////////////////////////////////////////////////////
        // Parser interface

        template <class I=nil, class P=nil> 
        struct rebind { typedef Parse_ListS<Parser,Sentinel,I,P> parser; };
        typedef Iterator byte_iterator;
        
        Parse_ListS();
        Parse_ListS(Iterator const & i);
        
        unsigned bytes() const;
        bool check(Iterator const & e) const;
        void init() const;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename Parser::template rebind<Iterator>::parser value_type;
        typedef Sentinel sentinel;
        typedef impl::Parse_ListS_iterator<value_type,sentinel,byte_iterator> iterator;
        typedef unsigned size_type;
        typedef int difference_type;
        typedef std::pair<iterator,iterator> range_type;

        template <class Container>
        struct wrapper { typedef Parse_ListS_wrapper<value_type, Sentinel, Container> t; };

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;
        range_type range() const;
        range_type value() const;

     private:
        template <class P, class S, class C> friend class Parse_ListS_wrapper;
    };

    /** \brief
        
        Holds a reference to the container !
      */
    template <class Parser, class Sentinel, class Container>
    class Parse_ListS_wrapper
        : public boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef Container container;
        typedef Sentinel sentinel;
        typedef typename Parser::byte_iterator byte_iterator;
        typedef Parser value_type;
        typedef impl::Parse_ListS_iterator<value_type,sentinel,byte_iterator> iterator;
        typedef unsigned size_type;
        typedef int difference_type;
        typedef std::pair<iterator,iterator> range_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        template <class P, class S, class I, class IP>
        Parse_ListS_wrapper(Parse_ListS<P,S,I,IP> const & list, Container & container);

        // no default constructor
        // no copy
        // default destructor
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Accessors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;
        range_type range() const;

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Mutators
        ///@{

        template <class Value> void insert(iterator pos, Value const & t); 
        template <class Value> void insert(iterator pos, size_type n, Value const & t); 
        template <class InputIterator> void insert(iterator pos, InputIterator f, InputIterator l); 
        
        void erase(iterator pos, size_type n=1); 
        void erase(iterator f, iterator l); 
        void clear(); 

        ///@}

    protected:

    private:
        
        size_type i_;
        Container & container_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ParseListS.cci"
#include "ParseListS.ct"
#include "ParseListS.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
