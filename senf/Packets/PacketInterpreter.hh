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
    \brief PacketInterpreter public header */

#ifndef HH_SENF_Packets_PacketInterpreter_
#define HH_SENF_Packets_PacketInterpreter_ 1

// Custom includes
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>
#include <senf/Utils/intrusive_refcount.hh>
#include <senf/Utils/TypeIdValue.hh>
#include <senf/Utils/Tags.hh>
#include "PacketData.hh"

//#include "PacketInterpreter.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    enum DumpPacketAnnotations_t { dumpAnnotations, withoutAnnotations };


    template <class PacketType> class PacketInterpreter;

    void intrusive_ptr_add_ref(PacketInterpreterBase const * p);
    void intrusive_ptr_release(PacketInterpreterBase const * p);


    /** \brief Internal: Base packet interpreter class

        \internal

        This is the base class for the persistent interpreter. This class encapsulates all the
        functionality accessible via the packet handle, most handle operations are just forwarded.
      */
    class PacketInterpreterBase
        : protected PacketData,
          public detail::packet::interpreter_list_base,
          public intrusive_refcount_t<PacketInterpreterBase>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::smart_pointer<PacketInterpreterBase>::ptr_t ptr;

        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;

        typedef boost::iterator_range<iterator> range;
        typedef boost::optional<range> optional_range;
        typedef optional_range no_range;

        enum Append_t { Append };
        enum Prepend_t { Prepend };

        /** \brief Internal: Abstract packet factory

            \internal

            This abstract class provides an abstract packet factory interface. It allows to call
            almost any one of the create / createAfter / createBefore static PacketInterpreter
            without static information on the type of packet to create.
         */
        struct Factory {
            virtual ~Factory();

            // Create completely new packet

            virtual ptr create() const = 0;
            virtual ptr create(senf::NoInit_t) const = 0;
            virtual ptr create(size_type size) const = 0;
            virtual ptr create(size_type size, senf::NoInit_t) const = 0;
            template <class ForwardReadableRange>
            ptr create(ForwardReadableRange const & range) const;
            virtual ptr create(byte * data, size_type size, size_type chunkSize = 0u,
                               size_type offset = 0u) = 0;

            // Create packet as new packet after a given packet

            virtual ptr createAfter(PacketInterpreterBase::ptr const & packet) const = 0;
            virtual ptr createAfter(PacketInterpreterBase::ptr const & packet, senf::NoInit_t) const = 0;
            virtual ptr createAfter(PacketInterpreterBase::ptr const & packet, size_type size) const = 0;
            virtual ptr createAfter(PacketInterpreterBase::ptr const & packet, size_type size,
                                    senf::NoInit_t) const = 0;
            template <class ForwardReadableRange>
            ptr createAfter(PacketInterpreterBase::ptr const & packet,
                            ForwardReadableRange const & range) const;

            // Create packet as new packet (header) const before a given packet

            virtual ptr createBefore(PacketInterpreterBase::ptr const & packet) const = 0;
            virtual ptr createBefore(PacketInterpreterBase::ptr const & packet, senf::NoInit_t) const = 0;

            virtual ptr createInsertBefore(PacketInterpreterBase::ptr const & packet) const = 0;
            virtual ptr createInsertBefore(PacketInterpreterBase::ptr const & packet, senf::NoInit_t) const = 0;

            // Parse next packet in chain

            virtual ptr parseNext(ptr const & packet, PacketInterpreterBase::optional_range const & range) const = 0;
        };

        typedef Factory const * factory_t;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // protected constructors
        // no copy
        // no conversion constructors

        virtual ~PacketInterpreterBase();

        static factory_t no_factory();

        ptr clone();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        //\{

        ptr next();
        ptr prev();
        ptr first();
        ptr last();

        template <class Type>
        typename PacketInterpreter<Type>::ptr parseNextAs();
        ptr parseNextAs(ptr const & self, factory_t factory, PacketInterpreterBase::optional_range const & range);
        template <class Type>
        typename PacketInterpreter<Type>::ptr as();

        ptr append(ptr packet);

        void reparse();

        template <class Type>
        typename PacketInterpreter<Type>::ptr replaceAs(difference_type offset, difference_type tailOffset);

        //\}

        ///\name Data access
        //\{

        using PacketData::valid;
        PacketData & data();

        //\}

        ///\name Annotations
        //\{

        template <class Annotation>
        Annotation & annotation();

        void clearAnnotations();

        //\}

        ///\name Access to the abstract interface
        //\{

        optional_range nextPacketRange();
        void finalizeThis();
        void finalizeTo(ptr other);
        void dump(std::ostream & os, DumpPacketAnnotations_t dumpAnnotationsSwitch);
        void memDebug(std::ostream & os);
        TypeIdValue typeId();
        factory_t factory();
        factory_t nextPacketType();

        //\}

    protected:
        // protected structors

        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);
        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, ptr before);

        ptr appendClone(detail::PacketImpl * impl, iterator base, iterator new_base);
        ptr appendClone(detail::PacketImpl * impl, range r);

        PacketInterpreterBase * nextP();
        PacketInterpreterBase * prevP();

    public:
        // Need this public for g++ < 4.0. Since PacketInterpreter is not publicly visible, it should not
        // be a real problem to make impl() public here
        detail::PacketImpl & impl() const;

    private:
        // abstract packet type interface

        virtual optional_range v_nextPacketRange() = 0;
        virtual ptr v_appendClone(detail::PacketImpl * impl, iterator base, iterator new_base) = 0;
        virtual ptr v_appendClone(detail::PacketImpl * impl, range r) = 0;
        virtual void v_finalize() = 0;
        virtual void v_dump(std::ostream & os) = 0;
        virtual TypeIdValue v_type() = 0;
        virtual factory_t v_factory() = 0;
        virtual factory_t v_nextPacketType() = 0;

        // reference/memory management. Only to be called by intrusive_refcount_t.

        void add_ref();
        void release();

        // containment management. Only to be called by PacketImpl.

        void assignImpl(detail::PacketImpl *);
        void releaseImpl();

        friend class detail::PacketImpl;
        friend class intrusive_refcount_base;
        template <class PacketType> friend class PacketInterpreter;
        friend struct detail::packet::test::TestDriver;
        friend class PacketParserBase;

        friend void senf::intrusive_ptr_add_ref(PacketInterpreterBase const *);
        friend void senf::intrusive_ptr_release(PacketInterpreterBase const *);
    };

    /** \brief Internal: Concrete packet interpreter

        \internal

        Instantiations of this class build the interpreter chain. This class is accessed by the
        packet handles. It provides the packet-type specific functionality in addition to the
        interface defined in the PacketInterpreterBase class.

        \see PacketTypeBase for the \a PacketType interface
      */
    template <class PacketType>
    class PacketInterpreter
        : public PacketInterpreterBase
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename senf::detail::packet::smart_pointer<
            PacketInterpreter>::ptr_t ptr;
        typedef PacketType type;
        typedef typename type::parser parser;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // private constructors
        // no copy
        // no conversion constructors

        static factory_t factory();

        // Create completely new packet

        static ptr create();
        static ptr create(senf::NoInit_t);
        static ptr create(size_type size);
        static ptr create(size_type size, senf::NoInit_t);
        template <class ForwardReadableRange>
        static ptr create(ForwardReadableRange const & range);
        static ptr create(byte * data, size_type size, size_type chunkSize = 0u,
                          size_type offset = 0u);

        // Create packet as new packet after a given packet

        static ptr createAfter(PacketInterpreterBase::ptr const & packet);
        static ptr createAfter(PacketInterpreterBase::ptr const & packet, senf::NoInit_t);
        static ptr createAfter(PacketInterpreterBase::ptr const & packet, size_type size);
        static ptr createAfter(PacketInterpreterBase::ptr const & packet, size_type size,
                               senf::NoInit_t);
        template <class ForwardReadableRange>
        static ptr createAfter(PacketInterpreterBase::ptr const & packet,
                               ForwardReadableRange const & range);

        // Create packet as new packet (header) before a given packet

        static ptr createBefore(PacketInterpreterBase::ptr const & spacket);
        static ptr createBeforeNoZero(PacketInterpreterBase::ptr const & spacket);
        static ptr createBefore(PacketInterpreterBase::ptr const & packet, senf::NoInit_t,
                                size_type space=0, size_type tailSpace=0);

        static ptr createInsertBefore(PacketInterpreterBase::ptr const & packet);
        static ptr createInsertBefore(PacketInterpreterBase::ptr const & packet, senf::NoInit_t);

        // Create a clone of the current packet

        ptr clone();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        // Packet field access

        parser fields();

        // PacketType access

        static size_type initSize();
        static size_type initHeadSize();

    protected:

    private:
        // Private structors

        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);
        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e,
                          PacketInterpreterBase::ptr const & before);

        static ptr create(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        static ptr create(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);
        static ptr create(detail::PacketImpl * impl, iterator b, iterator e,
                          PacketInterpreterBase::ptr const & before);

        // PacketType access

        void init();

        // virtual interface

        virtual optional_range v_nextPacketRange();
        virtual PacketInterpreterBase::ptr v_appendClone(detail::PacketImpl * impl,
                                                         iterator base, iterator new_base);
        virtual PacketInterpreterBase::ptr v_appendClone(detail::PacketImpl * impl, range r);
        virtual void v_finalize();
        virtual void v_dump(std::ostream & os);
        virtual TypeIdValue v_type();
        virtual factory_t v_factory();
        virtual factory_t v_nextPacketType();

        // factory

        /** \brief Internal: Implementation of abstract factory interface

            \internal

            Implements the abstract factory interface for \a PacketType
         */
        struct FactoryImpl : public Factory {
            FactoryImpl() {}

            // Create completely new packet

            virtual PacketInterpreterBase::ptr create() const;
            virtual PacketInterpreterBase::ptr create(senf::NoInit_t) const;
            virtual PacketInterpreterBase::ptr create(size_type size) const;
            virtual PacketInterpreterBase::ptr create(size_type size,senf::NoInit_t) const;
            virtual PacketInterpreterBase::ptr create(byte * data, size_type size,
                                                      size_type chunkSize = 0u, size_type offset = 0u);

            // Create packet as new packet after a given packet

            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr const & packet)
                const;
            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr const & packet,
                                                           senf::NoInit_t) const;
            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr const & packet,
                                                           size_type size) const;
            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr const & packet,
                                                           size_type size, senf::NoInit_t) const;

            // Create packet as new packet (header) before a given packet

            virtual PacketInterpreterBase::ptr createBefore(PacketInterpreterBase::ptr const & packet)
                const;
            virtual PacketInterpreterBase::ptr createBefore(PacketInterpreterBase::ptr const & packet,
                                                            senf::NoInit_t)
                const;

            virtual PacketInterpreterBase::ptr createInsertBefore(PacketInterpreterBase::ptr const & packet)
                const;
            virtual PacketInterpreterBase::ptr createInsertBefore(PacketInterpreterBase::ptr const & packet,
                                                                  senf::NoInit_t)
                const;

            // Parse next packet in chain

            virtual PacketInterpreterBase::ptr parseNext(PacketInterpreterBase::ptr const & packet, PacketInterpreterBase::optional_range const & range)
                const;
        };

        friend struct detail::packet::test::TestDriver;
        friend class PacketInterpreterBase;
        friend struct FactoryImpl;
    };

    /** \brief Invalid packet chain operation

        This exception signals an invalid operation on the chain like trying to find a non-existent
        chain member and other similar error conditions.
     */
    struct InvalidPacketChainException : public senf::Exception
    { InvalidPacketChainException() : senf::Exception("invalid packet chain") {} };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketInterpreter_i_)
#define HH_SENF_Packets_PacketInterpreter_i_
#include "PacketInterpreter.cci"
#include "PacketInterpreter.ct"
#include "PacketInterpreter.cti"
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
