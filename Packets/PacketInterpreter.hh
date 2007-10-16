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
    \brief PacketInterpreter public header */

#ifndef HH_PacketInterpreter_
#define HH_PacketInterpreter_ 1

// Custom includes
#include "../boost/intrusive/ilist.hpp"
#include <boost/optional.hpp>
#include <boost/range.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include "../Utils/intrusive_refcount.hh"
#include "../Utils/pool_alloc_mixin.hh"
#include "PacketData.hh"
#include "../Utils/TypeIdValue.hh"

//#include "PacketInterpreter.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    template <class PacketType> class PacketInterpreter;

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
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef senf::detail::packet::smart_pointer<
            PacketInterpreterBase>::ptr_t ptr;

        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;

        typedef boost::iterator_range<iterator> range;
        typedef boost::optional< boost::iterator_range<iterator> > optional_range;
        typedef optional_range no_range;

        enum Append_t { Append };
        enum Prepend_t { Prepend };
        enum NoInit_t { noinit };

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
            virtual ptr create(NoInit_t) const = 0;
            virtual ptr create(size_type size) const = 0;
            virtual ptr create(size_type size, NoInit_t) const = 0;
            template <class ForwardReadableRange>
            ptr create(ForwardReadableRange const & range) const;
            
            // Create packet as new packet after a given packet

            virtual ptr createAfter(PacketInterpreterBase::ptr packet) const = 0;
            virtual ptr createAfter(PacketInterpreterBase::ptr packet, NoInit_t) const = 0;
            virtual ptr createAfter(PacketInterpreterBase::ptr packet, size_type size) const = 0;
            virtual ptr createAfter(PacketInterpreterBase::ptr packet, size_type size, 
                                    NoInit_t) const = 0;
            template <class ForwardReadableRange>
            ptr createAfter(PacketInterpreterBase::ptr packet, 
                            ForwardReadableRange const & range) const;
            
            // Create packet as new packet (header) const before a given packet

            virtual ptr createBefore(PacketInterpreterBase::ptr packet) const = 0;
            virtual ptr createBefore(PacketInterpreterBase::ptr packet, NoInit_t) const = 0;

            // Parse next packet in chain

            virtual ptr parseNext(ptr packet) const = 0;
        };

        typedef Factory const * factory_t;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // protected constructors
        // no copy
        // no conversion constructors

        virtual ~PacketInterpreterBase();

        static                             factory_t no_factory();
        
        ptr clone();
 
        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///\name Interpreter chain access
        ///@{

        ptr next();
        ptr prev();
        ptr first();
        ptr last();

        template <class Type> typename PacketInterpreter<Type>::ptr parseNextAs();
                                       ptr                          parseNextAs(factory_t factory);
        template <class Type>          bool                         is();
        template <class Type> typename PacketInterpreter<Type>::ptr as();

        ptr append(ptr packet);

        ///@}

        ///\name Data access
        ///@{

        using PacketData::valid;
        PacketData & data();
        
        ///@}

        ///\name Access to the abstract interface
        ///@{

        optional_range nextPacketRange();
        void finalize();
        void dump(std::ostream & os);
        TypeIdValue typeId();
        factory_t factory();
        factory_t nextPacketType();

        ///@}

    protected:
        // protected structors

        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        PacketInterpreterBase(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);

        ptr appendClone(detail::PacketImpl * impl, iterator base, iterator new_base);
        ptr appendClone(detail::PacketImpl * impl, range r);

    public:
        // Need this for g++ < 4.0. Since PacketInterpreter is not publicly visible, it should not
        // be a real problem to make impl() public here
        using PacketData::impl;

    private:
        // abstract packet type interface

        virtual optional_range v_nextPacketRange() = 0;
        virtual ptr v_appendClone(detail::PacketImpl * impl, iterator base, iterator new_base) = 0;
        virtual ptr v_appendClone(detail::PacketImpl * impl, range r) =0;
        virtual void v_finalize() = 0;
        virtual void v_dump(std::ostream & os) = 0;
        virtual TypeIdValue v_type() = 0;
        virtual factory_t v_factory() = 0;
        virtual factory_t v_nextPacketType() = 0;

        // reference/memory management. Only to be called by intrusive_refcount_t.

        void add_ref();
        bool release();

        // containment management. Only to be called by PacketImpl.

        void assignImpl(detail::PacketImpl *);
        void releaseImpl();

        friend class detail::PacketImpl;
        friend class intrusive_refcount_t<PacketInterpreterBase>;
        template <class PacketType> friend class PacketInterpreter;
        friend class detail::packet::test::TestDriver;
        friend class PacketParserBase;
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
        : public PacketInterpreterBase,
          public pool_alloc_mixin< PacketInterpreter<PacketType> >
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename senf::detail::packet::smart_pointer<
            PacketInterpreter>::ptr_t ptr;
        typedef PacketType type;
        typedef typename type::parser parser;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // private constructors
        // no copy
        // no conversion constructors

        ~PacketInterpreter();

        static factory_t factory();

        // Create completely new packet

        static ptr create();
        static ptr create(NoInit_t);
        static ptr create(size_type size);
        static ptr create(size_type size, NoInit_t);
        template <class ForwardReadableRange>
        static ptr create(ForwardReadableRange const & range);

        // Create packet as new packet after a given packet

        static ptr createAfter(PacketInterpreterBase::ptr packet);
        static ptr createAfter(PacketInterpreterBase::ptr packet, NoInit_t);
        static ptr createAfter(PacketInterpreterBase::ptr packet, size_type size);
        static ptr createAfter(PacketInterpreterBase::ptr packet, size_type size, NoInit_t);
        template <class ForwardReadableRange>
        static ptr createAfter(PacketInterpreterBase::ptr packet, 
                               ForwardReadableRange const & range);

        // Create packet as new packet (header) before a given packet

        static ptr createBefore(PacketInterpreterBase::ptr packet);
        static ptr createBefore(PacketInterpreterBase::ptr packet, NoInit_t);

        // Create a clone of the current packet

        ptr clone();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        // Packet field access

        parser fields();
        parser * fields_p();

        // PacketType access

        static size_type initSize();
        static size_type initHeadSize();

    protected:

    private:
        // Private structors

        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        PacketInterpreter(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);

        static ptr create(detail::PacketImpl * impl, iterator b, iterator e, Append_t);
        static ptr create(detail::PacketImpl * impl, iterator b, iterator e, Prepend_t);

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
            // Create completely new packet

            virtual PacketInterpreterBase::ptr create() const;
            virtual PacketInterpreterBase::ptr create(NoInit_t) const;
            virtual PacketInterpreterBase::ptr create(size_type size) const;
            virtual PacketInterpreterBase::ptr create(size_type size,NoInit_t) const;
            
            // Create packet as new packet after a given packet

            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr packet) 
                const;
            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr packet, 
                                                           NoInit_t) const;
            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr packet, 
                                                           size_type size) const;
            virtual PacketInterpreterBase::ptr createAfter(PacketInterpreterBase::ptr packet, 
                                                           size_type size, NoInit_t) const;
            
            // Create packet as new packet (header) before a given packet

            virtual PacketInterpreterBase::ptr createBefore(PacketInterpreterBase::ptr packet) 
                const;
            virtual PacketInterpreterBase::ptr createBefore(PacketInterpreterBase::ptr packet,
                                                            NoInit_t) 
                const;

            // Parse next packet in chain

            virtual PacketInterpreterBase::ptr parseNext(PacketInterpreterBase::ptr packet) 
                const;
        };

        static const FactoryImpl factory_;

        parser * parser_p();

        boost::aligned_storage< sizeof(parser), 
                                boost::alignment_of<parser>::value > parserStorage_;

        friend class detail::packet::test::TestDriver;
        friend class PacketInterpreterBase;
        friend class FactoryImpl;
    };

    /** \brief Invalid packet chain operation

        This exception signals an invalid operation on the chain like trying to find a non-existent
        chain member and other similar error conditions. 
     */
    struct InvalidPacketChainException : public std::exception
    { virtual char const * what() const throw() { return "invalid packet chain"; } };
    
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_PacketInterpreter_i_)
#define HH_PacketInterpreter_i_
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
