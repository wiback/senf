// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

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
#include <boost/type_traits/aligned_storage.hpp>
#include <senf/Utils/pool_alloc_mixin.hh>
#include <senf/Utils/singleton.hh>

//#include "PacketImpl.mpp"
#include "PacketImpl.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

// Here we only need the size of PacketInterpreter, no implementation is needed
#ifndef HH_SENF_Packets_Packets__decls_
#define HH_SENF_Packets_Packets__decls_
#include "PacketInterpreter.hh"
#include "PacketType.hh"
#undef HH_SENF_Packets_Packets__decls_
#else
#include "PacketInterpreter.hh"
#include "PacketType.hh"
#endif

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
        zero-initialized (on the memory level) are elegible as fast annotations.

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

    template <class PacketType> class PacketInterpreter;

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
        PacketImpl(byte * data, size_type size, size_type chunkSize = 0u,
                   size_type offset = 0u);
        ~PacketImpl();

        // rerference/memory management

        void add_ref();
        bool release();
        refcount_t refcount() const;

        // Interpreter chain

        PacketInterpreterBase * first();
        PacketInterpreterBase * last();

        PacketInterpreterBase * next(PacketInterpreterBase * p);
        PacketInterpreterBase * prev(PacketInterpreterBase * p);

        void * allocateInterpreter();
        void deallocateInterpreter(void * address);

        bool usingExternalMemory() const;
        void releaseExternalMemory();

        void memDebug(std::ostream & os);

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

        // Annotations
        template <class Annotation>
        Annotation & annotation();

        void clearAnnotations();
        void assignAnnotations(PacketImpl const & other);
        void dumpAnnotations(std::ostream & os);

        /** \brief Internal: Keep PacketImpl instance alive

            \internal

            The Guard will keep the PacketImpl instance alive during a members execution time
            If the refcount should drop to 0, PacketImpl will be deleted after the member
            has completed executing.
         */
        struct Guard {
            Guard(PacketImpl * impl);
            ~Guard();
            PacketImpl * p;
        };

        static size_type maxPreallocHigh();
        static size_type maxPreallocHeapcount();

    private:
        void eraseInterpreters(interpreter_list::iterator b, interpreter_list::iterator e);
        void updateIterators(PacketData * self, difference_type pos, difference_type n);

        void * annotation(AnnotationRegistry::key_type key); // may return 0 !
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
        void * complexAnnotation(AnnotationRegistry::key_type key); // may return 0 !
        template <class Annotation>
        void * complexAnnotation();
#endif

#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
        void destroySelf();
#endif

        refcount_t refcount_;
        raw_container data_;
        interpreter_list interpreters_;

        // Annotations

        typedef boost::aligned_storage<SENF_PACKET_ANNOTATION_SLOTSIZE>::type SimpleAnnotationSlot;

#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
        typedef boost::ptr_vector< boost::nullable<AnnotationRegistry::EntryBase> >
            ComplexAnnotations;
        ComplexAnnotations complexAnnotations_;
#endif

        SimpleAnnotationSlot simpleAnnotations_[SENF_PACKET_ANNOTATION_SLOTS];

        // Preallocated interpreters

        union PreallocSlot {
            boost::aligned_storage<
                sizeof(PacketInterpreter<PacketTypeBase>),
                boost::alignment_of< PacketInterpreter<PacketTypeBase> >::value
                >::type storage_;
#ifndef SENF_PACKET_ALTERNATIVE_PREALLOC
            int nextOffset_;
#else
            PreallocSlot * nextFree_;
#endif
        };

        PreallocSlot prealloc_[SENF_PACKET_PREALLOC_INTERPRETERS];
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
        size_type preallocHigh_;
#endif
        PreallocSlot * preallocFree_;
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
        size_type preallocHeapcount_;
#endif

#ifdef SENF_DEBUG
        static size_type maxPreallocHigh_;
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
        static size_type maxPreallocHeapcount_;
#endif
#endif
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
