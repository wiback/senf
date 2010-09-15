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
    \brief PacketImpl public header */

#ifndef HH_SENF_Packets_PacketImpl_
#define HH_SENF_Packets_PacketImpl_ 1

// Custom includes
#include <memory>
#include <vector>
#include <boost/utility.hpp>
#include <boost/static_assert.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <senf/Utils/pool_alloc_mixin.hh>
#include <senf/Utils/singleton.hh>

//#include "PacketImpl.mpp"
#include "PacketImpl.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Marker base-class for complex annotations

        This class is used as a base class to mark an annotation type as complex. A complex
        annotation will have it's constructor/destructor called. Non-complex annotations will not
        have their constructor called, they will be zero initialized. The destructor of non-complex
        annotations is never called.

        An annotation must be marked as complex if it is not <a
        href="http://en.wikipedia.org/wiki/Plain_Old_Data_Structures">POD</a>. Simplified, an
        annotation must be marked as ComplexAnnotation, if

        \li it has a (user defined) constructor or destructor
        \li it has any data members which have (user defined) constructors or destructors

        \see \ref packet_usage_annotation
     */
    struct ComplexAnnotation {};

    /** \brief Dump annotation registry debug information

        This function will dump debug information about all registered annotations to \a os. This
        information may then be used to tune the following annotation parameters for optimal
        performance:
        \li \c SENF_PACKET_ANNOTATION_SLOTS (define, default 8) is the number of slots available for
            fast annotations
        \li \c SENF_PACKET_ANNOTATION_SLOTSIZE (define, default 16) is the maximum size of a fast
            annotation in bytes

        The output includes the current parameter and has the following columns:
        \li \c NAME: Annotation type name
        \li \c FAST: This is 'yes', if the annotation was allocated to a fast slot. Otherwise the
            annotation is managed as a slow/complex annotation
        \li \c COMPLEX: This is 'yes', if the annotation inherits from ComplexAnnotation
        \li \c SIZE: Size of the annotation in bytes

        Fast annotations are considerable faster than complex and slow annotations. However, only
        annotations which do not need constructor or destructor calls and which may be
        zero-initialized (on the memory level) are legible as fast annotations.

        It is thus desirable to eliminate any complex and slow annotations, if possible. To optimize
        the annotation system, you may take the following steps:
        \li If there are reasonably sized non-complex annotations which are larger than the current
            \c SENF_PACKET_ANNOTATION_SLOTSIZE value, increase this value accordingly
        \li If there are more non-complex annotations with a size less than
            \c SENF_PACKET_ANNOTATION_SLOTSIZE than there are available slots, increase \c
            SENF_PACKET_ANNOTATION_SLOTS accordingly
        \li If all fast annotations are smaller than \c SENF_PACKET_ANNOTATION_SLOTSIZE, you may
            decrease that value accordingly
        \li If there are fewer than \c SENF_PACKET_ANNOTATION_SLOTS fast annotations, you may
            decrease that value accordingly

        \see \ref packet_usage_annotation
     */
    void dumpPacketAnnotationRegistry(std::ostream & os);

namespace detail {

    /** \brief Internal: Packet data storage

        \internal

        This is the class holding the packet data and the interpreter chain. All manipulations of
        the packet data are performed via the interface exported here. This is very important, since
        PacketImpl will update the interpreters (that is the vector indices stored therein) whenever
        the data is changed.
     */
    class PacketImpl
        : boost::noncopyable,
          public pool_alloc_mixin<PacketImpl>
    {
    public:
        typedef senf::detail::packet::byte byte;
        typedef senf::detail::packet::raw_container raw_container;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::interpreter_list interpreter_list;
        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::refcount_t refcount_t;

        // structors

        PacketImpl();
        PacketImpl(size_type size, byte initValue);
        template <class InputIterator>
        PacketImpl(InputIterator b, InputIterator e);
        ~PacketImpl();

        // rerference/memory management

        void add_ref();
        void release();
        refcount_t refcount() const;

        // Interpreter chain

        PacketInterpreterBase * first();
        PacketInterpreterBase * last();

        PacketInterpreterBase * next(PacketInterpreterBase * p);
        PacketInterpreterBase * prev(PacketInterpreterBase * p);

        void appendInterpreter    (PacketInterpreterBase * p);
        void prependInterpreter   (PacketInterpreterBase * p);
        void prependInterpreter   (PacketInterpreterBase * p, PacketInterpreterBase * before);
        void truncateInterpreters (PacketInterpreterBase * p);
        void truncateInterpretersBackwards (PacketInterpreterBase * p);

        // Data container

        iterator begin();
        iterator end();
        size_type size();

        void insert(PacketData * self, iterator pos, byte v);
        void insert(PacketData * self, iterator pos, size_type n, byte v);
        template <class ForwardIterator>
        void insert(PacketData * self, iterator pos, ForwardIterator f, ForwardIterator l);

        void erase(PacketData * self, iterator pos);
        void erase(PacketData * self, iterator first, iterator last);
        void clear(PacketData * self);

        void reserve(size_type n);
        size_type capacity() const;

        // Annotations
        template <class Annotation>
        Annotation & annotation();

        void clearAnnotations();
        void assignAnnotations(PacketImpl const & other);
        void dumpAnnotations(std::ostream & os);

        /** \brief Internal: Keep PacketImpl instance alive

            \internal

            The Guard will keep the PacketImpl instance alive during a members execution time
            It the refcount should drop to 0, PacketImpl will be deleted after the member
            has completed executing.
         */
        struct Guard {
            Guard(PacketImpl * impl);
            ~Guard();
            PacketImpl * p;
        };

    private:
        void eraseInterpreters(interpreter_list::iterator b, interpreter_list::iterator e);
        void updateIterators(PacketData * self, difference_type pos, difference_type n);

        void * annotation(AnnotationRegistry::key_type key); // may return 0 !
        void * complexAnnotation(AnnotationRegistry::key_type key); // may return 0 !
        template <class Annotation>
        void * complexAnnotation();

        refcount_t refcount_;
        raw_container data_;
        interpreter_list interpreters_;

        union SimpleAnnotationSlot
        {
            unsigned char _ [SENF_PACKET_ANNOTATION_SLOTSIZE];
        };

        typedef boost::ptr_vector< boost::nullable<AnnotationRegistry::EntryBase> >
            ComplexAnnotations;
        ComplexAnnotations complexAnnotations_;

        SimpleAnnotationSlot simpleAnnotations_[SENF_PACKET_ANNOTATION_SLOTS];
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketImpl_i_)
#define HH_SENF_Packets_PacketImpl_i_
#include "PacketImpl.cci"
#include "PacketImpl.ct"
#include "PacketImpl.cti"
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
