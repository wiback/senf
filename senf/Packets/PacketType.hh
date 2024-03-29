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
    \brief PacketType public header */

#ifndef HH_SENF_Packets_PacketType_
#define HH_SENF_Packets_PacketType_ 1

// Custom includes
#include <iosfwd>
#include "PacketTypes.hh"
#include "PacketData.hh"
#include "PacketParser.hh"
#include "Packet.hh"

//#include "PacketType.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Helper base-class implementing the PacketType interface

        This helper class maybe used when building a new PacketType. It provides a complete default
        implementations of this interface. To define a new PacketType, derive from this class and
        implement the members you want to change.

        \code
        struct SomePacketType : public senf::PacketTypeBase
        {
            typedef senf::ConcretePacket<SomePacketType> packet;
            typedef SomePacketParser parser;

            static size_type initSize()
            {
                // This value can in most cases be taken from the parser
                return senf::init_size<parser>::value;
            }

            static size_type initHeadSize()
            {
                // optional member. This is only needed, if the packet has header and trailer.
                return constant_initial_size_of_header
            }

            static void init(packet p)
            {
                // Initialize a new packet by e.g. setting some fields. Should call
                // the packet parsers init() member
                p->init();
                p->some_field() = 1;
            }

            static optional_range nextPacketRange(packet p)
            {
                if (have_next_packet)
                    // return iterator range delimiting the packet, e.g.:
                    return range(p.data()+begin_offset, p.data()+end_offset);
                else
                    return no_range();
            }

            static factory_t nextPacketType(packet p)
            {
                if (have_next_packet && know_next_packet_type)
                    // \a NextPacket is the \c ConcretePacket instantiation of the next packet
                    return NextPacket::factory();
                else
                    return no_factory();
            }

            static void finalize(packet p)
            {
                // optionally complete the packet by generating auto-generated information
                // (like checksums)
            }

            static void dump(packet p, std::ostream & os)
            {
                // Write out a readable representation of the packet for debug purposes
            }
        }
        \endcode

        You may leave out any one of the members (you should however always define the \c
        packet typedef member)

        \ingroup packet_module
      */
    struct PacketTypeBase
    {
        typedef Packet packet;

        typedef senf::detail::packet::iterator iterator;
        typedef senf::detail::packet::const_iterator const_iterator;
        typedef senf::detail::packet::size_type size_type;
        typedef senf::detail::packet::difference_type difference_type;
        typedef senf::detail::packet::byte byte;

        typedef PacketInterpreterBase::range range;
        typedef PacketInterpreterBase::optional_range optional_range;
        typedef PacketInterpreterBase::factory_t factory_t;

        static optional_range no_range();
        static factory_t no_factory();
        template <class PacketType> static factory_t factory();

        typedef VoidPacketParser parser;
                                        ///< Parser to parser packet fields
                                        /**< This typedef has to be set to the parser of the packet

                                             The default is a VoidPacketParser which does not parse
                                             any field. */

        static size_type initSize();
                                        ///< Get size of new (empty) packet
                                        /**< The default implementation returns 0.
                                             \returns size that needs to be allocated to a newly
                                             constructed packet */

        static size_type initHeadSize();
                                        ///< Get size of new (empty) packet header
                                        /**< This function gives the index within a newly created,
                                             empty packet where a sub-packet is to be placed.

                                             The default implementation returns initSize().

                                             \implementation Ok, it does not really return
                                                 initSize(), it returns size_type(-1) which is
                                                 interpreted to mean initSize(). It can't return
                                                 initSize since initSize() is not (and can't be
                                                 since it is static) virtual. */

        static void init(packet p);
                                        ///< Initialize new packet
                                        /**< This member is called to initialize a just created new
                                             packet. The new packet will have a size of at least
                                             initSize() but the size may well be larger. It is also
                                             possible for the packet to already have further
                                             sub-packets.

                                             The default implementation does nothing. */



        static optional_range nextPacketRange(packet p);
                                        ///< Get next packet placement
                                        /**< nextPacketRange returns the iterator range where the
                                             next packet (header) is placed within the current
                                             packet.

                                             The default implementation always returns
                                             <tt>no_range()</tt>.

                                             \returns nextPacketRange must return
                                             \li <tt>\ref interpreter::range(b,e)</tt> (where \a b
                                                 and \a e are the iterators delimiting the next
                                                 packet range) if the packet allows a next
                                                 header. The returned range is allowed to be empty.
                                             \li <tt>\ref interpreter::no_range()</tt> (a
                                                 default constructed \c no_range instance) if
                                                 no next header is allowed */

        static factory_t nextPacketType(packet p);
                                        ///< Get type of next packet
                                        /**< nextPacketType retrieves the type of the next packet
                                             returning a factory to create that packet.

                                             The default implementation always returns
                                             <tt>no_factory()</tt>.

                                             \returns factory to create next packet
                                             \li <tt>interpreter::factory<OtherPacketType>()</tt> if
                                                 the type of the packet can be determined
                                             \li <tt>interpreter::no_factory()</tt> if the type of
                                                 the packet cannot be determined or no next packet
                                                 is supported */

        static void finalize(packet p);
                                        ///< Finalize packet
                                        /**< finalize() will be called to complete a packet after it
                                             has been modified. This function must calculate any
                                             checksums, set size fields from the interpreter chain
                                             etc.

                                             The default implementation does nothing. */

        static void dump(packet p, std::ostream & os);
                                        ///< Dump packet data in readable form
                                        /**< The dump() function writes out a complete
                                             representation of the packet. This is used for most for
                                             debugging purposes.

                                             The default implementation does nothing. */
    };


    /** \brief Mixin to provide standard implementations for nextPacketRange and nextPacketType

        This mixin class simplifies the definition of simple packets:

        \li The packets consist of three sections: The header, the payload and an optional trailer.
        \li If the packet has a trailer, both the header and the trailer must have a fixed size.

        This mixin provides the nextPacketRange() member as well as initSize() and init(). If you
        additionally provide the optional \a Registry argument, PacketTypeMixin provides a simple
        implementation of nextPacketType().

        When using the PacketTypeMixin, the implementation of a packet is simplified to:
        \code
        // Here 'SomeRegistryTag' is optional
        struct SimplePacketType
            : public senf::PacketTypeBase,
              public senf::PacketTypeMixin<SimplePacketType, SomeRegistryTag>
        {
            typedef senf::PacketTypeMixin<SimplePacketType, SomeRegistryTag> mixin;
            typedef senf::ConcretePacket<SimplePacketType> packet;
            typedef SomePacketParser parser;

            using mixin::nextPacketRange;
            using mixin::nextPacketType;  // Only if the optional 'Registry' argument is provided
            using mixin::initSize;
            using mixin::init;

            static key_t nextPacketKey(packet p)
            {
                // Return the key in the registry under which the next packet
                // header is to be found. This member must be given if a Registry argument is
                // passed to the PacketTypeMixin template.
                return p->typeField();
            }

            static void finalize(packet p)
            {
                // Set the type field by querying the type of the next packet. This is an
                // optional assignment: If the key is not found, the value returned by 'key'
                // is an empty optional and the assignment will be skipped.
                p->typeField << key(p.next(senf::nothrow));

                // optionally complete the packet by generating auto-generated information
                // (like checksums)
            }

            static void dump(packet p, std::ostream & os)
            {
                // Write out a readable representation of the packet for debug purposes
            }

            static interpreter::size_type initHeadSize()
            {
                // This member is optional. It returns the \e fixed header size if the packet has a
                // trailer.
                return header_size;
            }

        };
        \endcode

        Most of the members are optional, which reduces the minimal implementation of a packet to:

        \code
        struct SimplePacketType
            : public senf::PacketTypeBase,
              public senf::PacketTypeMixin<SimplePacketType, SomeRegistryTag>
        {
            typedef senf::PacketTypeMixin<SimplePacketType, SomeRegistryTag> mixin;
            typedef senf::ConcretePacket<SimplePacketType> packet;
            typedef SomePacketParser parser;

            using mixin::nextPacketRange;
            using mixin::nextPacketType;
            using mixin::initSize;
            using mixin::init;

            // 'typeField()' is one of the fields defined in the parser which holds
            // the next-header information

            static key_t nextPacketKey(packet p) { return p->typeField(); }
            static void finalize(packet p)       { p->typeField() << key(p.next(senf::nothrow)); }

            static void dump(packet p) {
                // should always be implemented although optional
            }
        };
        \endcode

        \ingroup packet_module
     */
    template <class Self, class Registry=void>
    class PacketTypeMixin
    {
    public:
        typedef typename Registry::key_t key_t;
        typedef boost::optional<key_t> optional_key_t;

        static optional_key_t key (Packet const & p); ///< Find key of packet from registry
                                        /**< key() will query the registry to find the key of the
                                             given packet. Whereas \c nextPacketKey() as implemented
                                             by the mixin user will provide the registry key of the
                                             next packet from information stored in the current
                                             packets header, the key() member will look up the type
                                             of packet \a p in the registry and return it's
                                             key.

                                             If either \a p is an in - valid() packet or the packet
                                             type is not found in the registry, the returned
                                             optional value will be empty. */

        static PacketTypeBase::factory_t lookup (key_t key); ///< Lookup the key in the registry
                                        /**< lookup() will query the registry and find the factory
                                             for the given key. If the key cannot be found,
                                             no_factory() will be returned. */

        ///\name PacketType interface implementation
        //\{

        static PacketInterpreterBase::optional_range nextPacketRange (ConcretePacket<Self> const & p);
        static PacketInterpreterBase::factory_t      nextPacketType  (ConcretePacket<Self> const & p);
        static PacketInterpreterBase::size_type      initSize        ();
        static void                                  init            (ConcretePacket<Self> const & p);

        //\}
    };

#   ifndef DOXYGEN

    template <class Self>
    class PacketTypeMixin<Self,void>
    {
    public:
        static PacketInterpreterBase::optional_range nextPacketRange (ConcretePacket<Self> const & p);
        static PacketInterpreterBase::size_type      initSize        ();
        static void                                  init            (ConcretePacket<Self> const & p);
    };

#   endif

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketType_i_)
#define HH_SENF_Packets_PacketType_i_
#include "PacketType.cci"
#include "PacketType.ct"
#include "PacketType.cti"
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

