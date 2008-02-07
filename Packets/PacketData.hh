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
    \brief PacketData public header */

#ifndef HH_PacketData_
#define HH_PacketData_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "../Utils/safe_bool.hh"
#include "PacketTypes.hh"

//#include "PacketData.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Packet data STL-sequence view

        The PacketData class provides an STL-sequence compatible view of the raw packet data. Each
        packet/header/interpreter in the chain references the same storage area, presenting a
        different (but nested/overlapping) section of the data.

        Whenever the data is manipulated through PacketData, the change is assumed to be within the
        data range of that packet: All insertions take place \e inside \c this packet and \e outside
        any following packets in the packet chain. 

        \warning It is not permissible to change data belonging to a following
            packet/header/interpreter even though this data is part of \c this sequence. Doing so
            will corrupt the packet data.
        
        \par

        \warning When accessing packet data via the PacketData interface you are on your own: The
            packet is not validated in any way, you bypass all parsers.

        All public members are those of an STL random-access sequence.

        \implementation This class is very tightly integrated with PacketInterpreterBase /
            PacketInterpreter. It is separated out of those classes primarily to provide a clean
            sequence interface to the library user and not for implementation reasons (it would have
            been simpler to implement all these members in PacketInterpreterBase).

        \ingroup packet_module
      */
    class PacketData
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;
        typedef byte value_type;
        typedef byte & reference;
        typedef byte const & const_reference;
        typedef byte * pointer;
        typedef byte const * const_pointer;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no public constructors
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////
    
        ///\name Sequence interface to raw data
        ///@{

        iterator begin() const; ///< Return iterator to beginning
                                /**< Returns an <em>random access iterator</em> referring to the
                                     first byte of the packet data. */
        iterator end() const; ///< Return iterator to end 
                              /**< Returns an <em>random access iterator</em> referring to the 
                                   byte past the end of the packet data. */
        size_type size() const; ///< Returns the number of bytes in the packet data.
        bool empty() const; ///< Test whether the packet data is empty.
                            /**< Returns whether the packet data is empty, i.e. whether its size
                                 is 0. This function does not modify the content of the packet 
                                 data in any way. To clear the content use clear() */        
        byte operator[](size_type n) const; ///< Access byte in the packet data
        byte & operator[](size_type n); ///< Access byte in the packet data

        // Modifying the raw packet data

        // IMPORTANT NOTE: It is not possible to insert data AFTER an empty packet
        // since for an empty packet begin() == end(). However, I hope this problem is
        // only academic since what should an empty packet be good for ?
        void insert(iterator pos, byte v);
        void insert(iterator pos, size_type n, byte v);
#       ifndef DOXYGEN
        template <class InputIterator>
        void insert(iterator pos, InputIterator f, InputIterator l,
                    typename boost::disable_if< boost::is_convertible<InputIterator,size_type> >::type * = 0);
#       else
        template <class InputIterator>
        void insert(iterator pos, InputIterator f, InputIterator l);
#       endif

        void erase(iterator pos);
        void erase(iterator first, iterator last);
        void clear(); ///< All bytes of the packet data dropped, leaving the container with a size of 0. */
        
        void resize(size_type n, byte v=0);

        ///@}

    protected:
        PacketData(size_type b, size_type e);

        /// Need to make this protected so we can change it in the derived class
        detail::PacketImpl * impl_;

        detail::PacketImpl & impl() const;

        bool valid();

    private:
        size_type begin_;
        size_type end_;

        friend class detail::PacketImpl;
    };

    class PacketParserBase;

    /** \brief Invalid packet data access

        This exception is signaled whenever an operation tries to access an out-of-bounds data
        byte. If the packet has been implemented correctly, this signals a malformed packet.
     */
    struct TruncatedPacketException : public std::exception
    { virtual char const * what() const throw() { return "truncated packet"; } };

    /** \brief Re-validating data iterator

        This class is a wrapper around a PacketData::iterator instance. It will revalidate the
        iterator on every access. This keeps the iterator valid even when the data container is
        resized and thereby possibly relocated. The iterator will always point to the byte at the
        same offset from the packets beginning. If data is inserted before this iterators position,
        the data pointed to will of course change.

        For this to work, the safe_data_iterator must be initialized with the container to which the
        iterator belongs. After this initialization it can be used like any other iterator.
     */
    class safe_data_iterator
        : public boost::iterator_facade< safe_data_iterator,
                                         PacketData::value_type,
                                         boost::random_access_traversal_tag >,
          public comparable_safe_bool<safe_data_iterator>
    {
    public:
        typedef PacketData::size_type size_type;

        safe_data_iterator(); ///< Make uninitialized iterator
        explicit safe_data_iterator(PacketData & data); 
                                        ///< Construct iterator only setting the data container
        safe_data_iterator(PacketData & data, PacketData::iterator i);
                                        ///< Initialize iterator to given position
        explicit safe_data_iterator(PacketParserBase const & parser);
                                        ///< Initialize iterator from parser
                                        /**< The iterator will point to the parsers start
                                             position. */

        safe_data_iterator & operator=(PacketData::iterator i); ///< Assign iterator
                                        /**< The iteator \a i must be from the container wo which \c
                                             this iterator has been initialized. */
        safe_data_iterator & operator=(PacketParserBase const & parser);
                                        ///< Assign iterator from parser
                                        /**< The iterator will point to the parser start
                                             position. */

        operator PacketData::iterator() const; ///< Convert to iterator

        bool boolean_test() const;      ///< Check, if iterator is initialized

        PacketData & data() const;      ///< Access data container

    private:
        friend class boost::iterator_core_access;

        // iterator_facade interface

        value_type & dereference() const;
        bool equal(safe_data_iterator const & other) const;
        difference_type distance_to(safe_data_iterator const & other) const;
        void increment();
        void decrement();
        void advance(difference_type n);

        PacketData::iterator i() const;

        PacketData * data_;
        size_type i_;
    };
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_PacketData_i_)
#define HH_PacketData_i_
#include "PacketData.cci"
//#include "PacketData.ct"
#include "PacketData.cti"
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

