//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ListParser public header */

#ifndef HH_SENF_Packets_ListParser_
#define HH_SENF_Packets_ListParser_ 1

// Custom includes
#include <boost/utility/enable_if.hpp>
#include "PacketParser.hh"

//#include "ListParser.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    namespace detail { template <class Container> class ListParser_Iterator; }

    template <class ListPolicy>
    class ListParser_Container;

    /** \brief Arbitrary sequential element collection

        A List is a sequential collection of elements. The element type is given as an arbitrary
        parser. The list is more flexible than a vector: It is not limited to fixed-size elements
        and it might not have direct access to the size of the collection.

        The cost is however, that a List is only a model of an STL forward sequence. The parser
        provides a reduced interface to this sequence, the container wrapper provides the complete
        interface.

        ListParser makes use of a policy template argument, \a ListPolicy, to customize the way the
        list is laid out. This policy is given quite some freedom in the list implementation. It is
        however important, that list elements <em>always follow each other without padding</em> (if
        padding is needed, it needs to be part of the element parser).

        You will normally not instantiate ListParser directly, you will use the \ref
        SENF_PARSER_LIST() helper macro.

        Some basic list access methods are defined as parser members. To access the complete list
        API however you will need to instantiate a container wrapper for the list. See \ref
        packet_usage_fields_collection.

        \see
            How to access \ref packet_usage_fields_collection \n
            SENF_PARSER_LIST() macro used to define list fields \n
            ListParser_Container list container wrapper API \n
            ExampleListPolicy

        \ingroup parsecollection
      */
    template <class ListPolicy>
    class ListParser
        : public PacketParserBase,
          private ListPolicy
    {
    public:
        typedef ListPolicy policy;

        ListParser(data_iterator i, state_type s);
        ListParser(ListPolicy policy, data_iterator i, state_type s);
                                        ///< Additional policy specific constructor
                                        /**< This constructor may be used, if the policy needs
                                             additional parameters. */

        size_type bytes() const;
        void init() const;

        static const size_type init_bytes = ListPolicy::init_bytes;

        //-////////////////////////////////////////////////////////////////////////
        // Container interface

        typedef typename ListPolicy::element_type value_type;
        typedef typename ListPolicy::container_type container_type;

        container_type container() const;

        size_type size() const;
        bool empty() const;

        value_type front() const;
        value_type back() const;

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Range>
        void assign(Range const & range);

        template <class Value> void push_back        (Value const & value, size_type n=1) const;
        value_type                  push_back_space  (size_type n=1) const;
        template <class Value> void push_front       (Value const & value, size_type n=1) const;
        value_type                  push_front_space (size_type n=1) const;
                               void resize           (size_type n) const;
        template <class Value> void resize           (size_type n, Value value) const;

        static ListParser & get(ListPolicy & p);
        static ListParser const & get(ListPolicy const & p);

    private:
        template <class Policy> friend class ListParser_Container;
    };

    /** \brief ListParser container wrapper

        This is the container wrapper used for list parsers. The container wrapper will stay valid
        after changing the collection. However the container still depends on the packet and will be
        invalidated if the Packet is deallocated or if the packet size is changed from without the
        container wrapper (more precisely, it is invalidated if the insertion/deletion happens before
        the vector in the packet data).

        The vector container wrapper provides a complete STL random-access sequence interface.

        \code
        SomePacket p (...);
        SomePacket::aListCollection_t::container_type c (p->aListCollection());
        c.insert(c.begin(), ... );
        \endcode

        \see ListParser
      */
    template <class ListPolicy>
    class ListParser_Container
        : private ListPolicy
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef ListPolicy policy;
        typedef typename ListPolicy::parser_type parser_type;
        typedef PacketParserBase::data_iterator data_iterator;
        typedef PacketParserBase::size_type size_type;
        typedef PacketParserBase::difference_type difference_type;
        typedef typename ListPolicy::element_type value_type;
        typedef detail::ListParser_Iterator<ListParser_Container> iterator;
        typedef iterator const_iterator;
        typedef PacketParserBase::state_type state_type;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // no default constructor
        // default copy
        // default destructor
        // conversion constructors

        ListParser_Container(parser_type const & list);
        ~ListParser_Container();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        ///\name Accessors
        //\{

        size_type size() const;
        bool empty() const;

        iterator begin() const;
        iterator end() const;

        value_type front() const;
        value_type back() const;

        //\}
        ///\name Mutators
        //\{

        // All these operations can be quite inefficient depending on the list type
        value_type shift(iterator pos, size_type n=1);
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

        template <class Value> void push_back        (Value const & value, size_type n=1);
        value_type                  push_back_space  (size_type n=1);
        template <class Value> void push_front       (Value const & value, size_type n=1);
        value_type                  push_front_space (size_type n=1);
        void                        resize           (size_type n);
        template <class Value> void resize           (size_type n, Value value);

        //\}

        ///\name Parser interface
        //\{

        parser_type parser() const;
        data_iterator i() const;
        state_type state() const;
        PacketData & data() const;

        size_type bytes() const;
        void init() const;

        //\}

    private:
        friend class detail::ListParser_Iterator<ListParser_Container>;

        state_type state_;
        size_type i_;
    };

    /** \brief Define ListParser field

        This macro is a special helper to define a senf::ListParser type field, a list of elements
        of type \a elt_type (a parser)  which size is determined by \a size.

        \code
        // The size field should be declared private or read-only (size is accessible via the list)
        SENF_PARSER_PRIVATE_FIELD ( list_size_, senf::UInt16Parser );
        // Define the list
        SENF_PARSER_LIST ( list, list_size_, EltParser );
        \endcode

        Here \c EltParser can be an arbitrary parser and need not have a fixed size.

        \warning Realize, that the \a size field is controlled by the list parser. This field
            should therefore be declared either read-only or private and must be changed only via
            the list parser.

        Further additional tags are supported which modify the type of list created:

        <table class="senf fixedcolumn">
        <tr><td>\c bytes(\a size)</td><td>\a size gives the size of the list in bytes not the
        number of contained elements</td></tr>

        <tr><td>\c packetSize()</td><td>Use the size of the packet to get the list size. The
        list will occupy all space up to the end of the packet.</td></tr>

        <tr><td>\c transform(\a transform, \a size)</td><td>The \a transform is applied to the \a
        size value, the value is not used directly</td>

        <tr><td>\c transform(\a transform, \c bytes(\a size))</td><td>The \a transform is applied to
        the \a size value. The value is then interpreted containing the list size in bytes not
        number of elements</td>
        </table>

        The optional \a transform is a class with the following layout
        \code
        struct MyTransform
        {
            typedef ... value_type;
            static value_type get(other_type v);
            static other_type set(value_type v);
        };
        \endcode

        \c other_type is \a size ::\c value_type, the type of the value returned by the \a size
        field, whereas the \c value_type typedef is the arbitrary return type of the transform.

        The tags are applied to the \a size parameter:
        \code
        SENF_PARSER_LIST ( list, transform(MyTransform, list_size_), EltParser );
        \endcode

        \warning There are some caveats when working with \c bytes() type lists:
        \li You may <b>only change the size of a contained element from a container wrapper</b>.
        \li While you hold a container wrapper, <b>only access the packet through this wrapper</b>
            or a nested wrapper either for reading or writing.

        \warning If lists are nested, you need to allocate a container wrapper for each level and
        may only access the packet through the lowest-level active container wrapper.

        \implementation These restrictions are necessary to ensure correct recalculation of the
            <tt>bytes</tt> field. For more info, see the comments in \ref ListBParser.ih

        \param[in] name field name
        \param[in] size name of field giving the list size
        \param[in] elt_type list element type

        \see
            How to use \ref packet_usage_fields_collection \n
            senf::ListParser the list parser API for list field access
            senf::ListParser_Container the list parser container API for list field access

        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_LIST(name, size, elt_type) \
        SENF_PARSER_LIST_I(public, name, size, elt_type)

    /** \brief Define private ListParser field

        \see \ref SENF_PARSER_LIST()

        \hideinitializer
        \ingroup packetparsermacros
     */
#   define SENF_PARSER_PRIVATE_LIST(name, size, elt_type) \
        SENF_PARSER_LIST_I(protected, name, size, elt_type)


    template <class ListPolicy, class OutputIterator>
    void copy(ListParser_Container<ListPolicy> container, OutputIterator result);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_ListParser_i_)
#define HH_SENF_Packets_ListParser_i_
//#include "ListParser.cci"
#include "ListParser.ct"
#include "ListParser.cti"
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
