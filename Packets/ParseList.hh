// $Id$
//
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
    \brief ParseList public header */

#ifndef HH_ParseList_
#define HH_ParseList_ 1

// Custom includes
#include <boost/utility.hpp>
#include "PacketParser.hh"

//#include "ParseList.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    namespace detail { template <class ElementParser, class IteratorPolicy> 
                       class Parse_List_Iterator; }

    template <class ListPolicy>
    class Parse_List_Container;

    /** \brief Arbitrary sequential element collection

        A List is a sequential collection of elements. The element type is given as an arbitrary
        parser. The list is more flexible than a vector: It is not limited to fixed-size elements
        and it might not have direct access to the size of the collection. 

        The cost is however, that a List is only a model of an STL forward sequence. The parser
        provides a reduced interface to this sequence, the container wrapper provides the complete
        interface.

        Pare_List makes use of a policy template argument, \a ListPolicy, to customize the way the
        list is laid out. This policy is given quite some freedom in the list
        implementation. It is however important, that list elements <em>always follow each other
        without padding</em> (if padding is needed, it needs to be part of the element parser).

        \see ExampleListPolicy
        \ingroup parsecollection
      */
    template <class ListPolicy>
    class Parse_List 
        : public PacketParserBase,
          private ListPolicy
    {
    public:
        Parse_List(data_iterator i, state_type s);
        Parse_List(ListPolicy policy, data_iterator i, state_type s);
                                        ///< Additional policy specific constructor
                                        /**< This constructor may be used, if the policy needs
                                             additional parameters. */

        size_type bytes() const;
        void init() const;

        static const size_type init_bytes = ListPolicy::init_bytes;

        ///////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename ListPolicy::element_type value_type;
        typedef detail::Parse_List_Iterator< 
            value_type, typename ListPolicy::iterator_policy > iterator;
        typedef iterator const_iterator;
        typedef typename ListPolicy::container_type container;

        size_type size() const;
        bool empty() const;
        
        iterator begin() const;
        iterator end() const;

        value_type front() const;
        value_type back() const;
        
        template <class Value> void push_back        (Value value, size_type n=1) const;
                               void push_back_space  (size_type n=1) const;
        template <class Value> void push_front       (Value value, size_type n=1) const;
                               void push_front_space (size_type n=1) const;
                               void resize           (size_type n) const;
        template <class Value> void resize           (size_type n, Value value) const;

    private:
        template <class Policy> friend class Parse_List_Container;
    };

    /** \brief Parse_List container wrapper

        This is the container wrapper used for list parsers. The container wrapper will stay valid
        after changing the collection. However the container still depends on the packet and will be
        invalidated if the Packet is deallocated or if the packet size is changed from without the
        container wrapper (more precisely, it is invalided if the insertion/deletion happens before
        the vector in the packet data).

        The vector container wrapper provides a complete STL random-access sequence interface.
        
        \code
        SomePacket p (...);
        SomePacket::aListCollection_t::container c (p->aListCollection());
        c.insert(c.begin(), ... );
        \endcode

        \see Parse_List
      */
    template <class ListPolicy>
    class Parse_List_Container
        : private ListPolicy
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename ListPolicy::parser_type parser_type;
        typedef PacketParserBase::data_iterator data_iterator;
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::difference_type difference_type;
        typedef typename ListPolicy::element_type value_type;
        typedef detail::Parse_List_Iterator<
            value_type, typename ListPolicy::iterator_policy> iterator;
        typedef iterator const_iterator;
        typedef PacketParserBase::state_type state_type;
        
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy
        // default destructor
        // conversion constructors

        Parse_List_Container(parser_type const & list);
        ~Parse_List_Container();
        
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Accessors
        ///@{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type front() const;
        value_type back() const;

        ///@}
        ///\name Mutators
        ///@{

        // All these operations can be quite inefficient depending on the list type
        void shift(iterator pos, size_type n=1);
        template <class Value>
        void insert(iterator pos, Value const & t);
        template <class Value>
        void insert(iterator pos, size_type n, Value const & t);
        template <class ForwardIterator>
#       ifndef DOXYGEN
        void insert(iterator pos, ForwardIterator f, ForwardIterator l,
                    typename boost::disable_if< boost::is_convertible<ForwardIterator,size_type> >::type * = 0);
#       else
        void insert(iterator pos, ForwardIterator f, ForwardIterator l);
#       endif

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

    private:
        state_type state_;
        size_type i_;
    };

        
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_ParseList_i_)
#define HH_ParseList_i_
//#include "ParseList.cci"
#include "ParseList.ct"
#include "ParseList.cti"
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
