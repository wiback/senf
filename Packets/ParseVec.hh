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
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp>
#include <boost/range.hpp>
#include <boost/type_traits.hpp>
#include "PacketParser.hh"
#include "ParseArray.hh" // for Parse_Array_iterator

//#include "ParseVec.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    template <class ElementParser, class Sizer> class Parse_Vector_Container;

    /** \brief
        
        \todo Make the sizer a private baseclass to profit from the empty-base-class optimization
     */
    template <class ElementParser, class Sizer>
    struct Parse_Vector : public PacketParserBase
    {
        Parse_Vector(data_iterator i, state_type s);
        Parse_Vector(Sizer sizer, data_iterator i, state_type s);

        size_type bytes() const;
        void init() const;

        static const size_type init_bytes = Sizer::init_bytes;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef ElementParser value_type;
        typedef detail::Parse_Array_iterator<value_type> iterator;
        typedef iterator const_iterator;
        typedef Parse_Vector_Container<ElementParser,Sizer> container;

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;
        value_type front() const;
        value_type back() const;

        // Mutators
        
        // The mutators provided here are those which don't take an iterator argument.
        // If you need to pass an iterator it is much simpler and cleaner to use the
        // 'container' wrapper
                   
        template <class Value> void push_back        (Value value, size_type n=1) const;
                               void push_back_space  (size_type n=1) const;
        template <class Value> void push_front       (Value value, size_type n=1) const;
                               void push_front_space (size_type n=1) const;
                               void resize           (size_type n) const;
        template <class Value> void resize           (size_type n, Value value) const;

     private:
        Sizer sizer_;

        friend class Parse_Vector_Container<ElementParser,Sizer>;
    };

    namespace detail { template <class SizeParser> class Parse_VectorN_Sizer; }

    template <class ElementParser, class SizeParser>
    struct Parse_VectorN
    {
        typedef Parse_Vector< ElementParser,
                              detail::Parse_VectorN_Sizer<SizeParser> > parser;
    };

    /** \brief

        Holds a reference to the container !
      */
    template <class ElementParser, class Sizer>
    class Parse_Vector_Container
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef Parse_Vector<ElementParser,Sizer> parser_type;
        typedef PacketParserBase::data_iterator data_iterator;
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::difference_type difference_type;
        typedef ElementParser value_type;
        typedef detail::Parse_Array_iterator<value_type> iterator;
        typedef iterator const_iterator;
        typedef PacketParserBase::state_type state_type;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // conversion constructors

        Parse_Vector_Container(parser_type const & vector);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Accessors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type operator[](difference_type i) const;
        value_type front() const;
        value_type back() const;

        ///@}
        ///\name Mutators
        ///@{

        iterator shift(iterator pos, size_type n=1);
        template <class Value>
        void insert(iterator pos, Value const & t);
        template <class Value>
        void insert(iterator pos, size_type n, Value const & t);
        template <class ForwardIterator>
        void insert(iterator pos, ForwardIterator f, ForwardIterator l,
                    typename boost::disable_if< boost::is_convertible<ForwardIterator,size_type> >::type * = 0);

        void erase(iterator pos, size_type n=1);
        void erase(iterator f, iterator l);
        void clear();

        template <class Value> void push_back        (Value value, size_type n=1);
                               void push_back_space  (size_type n=1);
        template <class Value> void push_front       (Value value, size_type n=1);
                               void push_front_space (size_type n=1);
                               void resize           (size_type n);
        template <class Value> void resize           (size_type n, Value value);

        ///@}

        ///\name Parser interface
        ///@{

        parser_type parser() const;
        data_iterator i() const;
        state_type state() const;
        PacketData & data() const;

        size_type bytes() const;
        void init() const;
        
        ///@}

    protected:

    private:
        void setSize(size_type value);

        Sizer sizer_;
        state_type state_;
        size_type i_;
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
// compile-command: "scons -u test"
// End:
