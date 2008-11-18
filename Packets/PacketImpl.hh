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
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/static_assert.hpp>
#include "../Utils/pool_alloc_mixin.hh"
#include "PacketTypes.hh"
#include "../Utils/singleton.hh"

//#include "PacketImpl.mpp"
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

namespace detail {

    struct AnnotationP
    {
        virtual ~AnnotationP();
    };

    template <class Annotation>
    struct TAnnotationP
        : public AnnotationP
    {
        Annotation annotation;
    };

    union AnnotationEntry {
        AnnotationP * p;
        unsigned long long i;
    };

    struct AnnotationIndexerBase
    {
        static unsigned maxAnnotations;
        static std::vector<bool> & small();
    };

    template <class Annotation>
    struct AnnotationIndexer 
        : public senf::singleton< AnnotationIndexer<Annotation> >, 
          public AnnotationIndexerBase
    {
        AnnotationIndexer();
        unsigned index_;
        static unsigned index();
        static bool const Complex = boost::is_base_of<ComplexAnnotation, Annotation>::value;
        static bool const Small = (sizeof(Annotation) <= sizeof(AnnotationEntry) && ! Complex);

#       ifdef BOOST_HAS_TYPE_TRAITS_INTRINSICS

        BOOST_STATIC_ASSERT(( (boost::has_trivial_constructor<Annotation>::value 
                               && boost::has_trivial_destructor<Annotation>::value)
                              || Complex ));

#       endif
    };

    template <class Annotation, bool Small = AnnotationIndexer<Annotation>::Small>
    struct GetAnnotation
    {
        static Annotation & get(AnnotationEntry & e);
    };

    template <class Annotation>
    struct GetAnnotation<Annotation, true>
    {
        static Annotation & get(AnnotationEntry & e);
    };

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

        void add_ref(refcount_t n=1);
        void release(refcount_t n=1);
        refcount_t refcount() const;

        // Interpreter chain

        PacketInterpreterBase * first();
        PacketInterpreterBase * last();

        PacketInterpreterBase * next(PacketInterpreterBase * p);
        PacketInterpreterBase * prev(PacketInterpreterBase * p);

        void appendInterpreter    (PacketInterpreterBase * p);
        void prependInterpreter   (PacketInterpreterBase * p);
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
        refcount_t refcount_;
        raw_container data_;
        interpreter_list interpreters_;
        
        typedef std::vector<AnnotationEntry> Annotations;
        Annotations annotations_;

        void eraseInterpreters(interpreter_list::iterator b, interpreter_list::iterator e);
        void updateIterators(PacketData * self, difference_type pos, difference_type n);
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketImpl_i_)
#define HH_SENF_Packets_PacketImpl_i_
#include "PacketImpl.cci"
//#include "PacketImpl.ct"
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

