// $Id$
//
// Copyright (C) 2006
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
    \brief PacketRegistry public header */

#ifndef HH_SENF_Packets_PacketRegistry_
#define HH_SENF_Packets_PacketRegistry_ 1

// Custom includes
#include <map>
#include <boost/utility.hpp> // for boost::noncopyable
#include <boost/optional.hpp>
#include <boost/preprocessor/cat.hpp>
#include <senf/Utils/Exception.hh>
#include "Packet.hh"

#include "PacketRegistry.ih"
//#include "PacketRegistry.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief %Packet registration facility

        The %PacketRegistry provides a generic facility to associate an arbitrary key with
        Packets. Example keys are Ethertype or IP protocols.

        Every %PacketRegistry is identified by a type tag:
        \code
        struct SomeTag {
            typedef some_key_type key_t;
        };
        \endcode
        The key type can be an arbitrary value type. The %PacketRegistry for this Tag can then be
        accessed using <code>senf::PacketRegistry<SomeTag>::</code>.

        The %PacketRegistry class has only static members and provides access to the packet
        registry. It allows two-way lookup either by key or by packet type.

        \code
        senf::Packet::factory_t factory (senf::PacketRegistry<SomeTag>::lookup(some_key).factory());
        SomeTag::key_t key = PacketRegistry<SomeTag>::key<SomePacket>();
        \endcode

        Packets can be registered either dynamically or statically. Dynamic:
        \code
        // dynamic registration
        senf::PacketRegistry<SomeTag>::registerPacket<SomePacket>(key_of_somePacket);

        // static registration. 'registerSomePacket' is an arbitrary symbol name
        SENF_PACKET_REGISTRY_REGISTER( SomeTag, key_of_somePacket, SomePacket );
        \endcode

        SENF_PACKET_REGISTRY_REGISTER will declare an anonymous global variable which will ensure,
        the packet is registered automatically during global initialization (as long as the object
        file is linked into the final executable).

        \section packet_registry_priority Multiple registration for a single key

        Ordinarily, the PacketRegistry will reject registering the same key twice. However, the
        registry supports an additional priority parameter when registering a packet. You may
        register multiple Packets with the same \a key as long as the \a priority is unique. The
        registration with the highest \a priority value will take precedence on key lookup.

        \ingroup packet_module
     */
    template <class Tag>
    class PacketRegistry
    {
    public:
        typedef typename detail::PacketRegistryImpl<typename Tag::key_t>::iterator iterator;
        typedef typename detail::PacketRegistryImpl<typename Tag::key_t>::Entry Entry;

        /** \brief Statically register a packet type in a PacketRegistry

            This class is normally used via SENF_PACKET_REGISTRY_REGISTER. To use this class
            directly, define a symbol in the following way:
            \code
            namespace {
                senf::PacketRegistry<Tag>::ScopedRegistrationProxy<PacketType>
                    registerPacketType (key, optional_priority);
            }
            \endcode
            Here  \a Tag  is the type  tag of the  registry to  register the packet  in, \a
            PacketType is the packet to register (this  is the ConcretePacket of that packet) and \a
            key is  the key of  type \c Tag::key_t  under which the packet  is to be  registered. \a
            registerPacketType is an arbitrary name for the global symbol.

            The packet will be registered in the constructor and will be unregistered when the scope
            of \c registerPacketType ends.
         */
        template <class PacketType>
        struct ScopedRegistrationProxy
        {
            ScopedRegistrationProxy(typename Tag::key_t key, int priority=0);
            ~ScopedRegistrationProxy();
        };

        /** \brief Register new packet type

            Register \a PacketType in the packet registry \a Tag under the given \a key.

            \par Preconditions:
                The given pair \a key, \a priority must be unique and not be assigned to any other
                packet class in this registry.  The %Packet must not already be registered in the
                registry.

            \tparam PacketType ConcretePacket instantiation of packet to register
            \param[in] key The key of the packet
            \param [in] priority Optional priority
         */
        template <class PacketType>
        static void registerPacket(typename Tag::key_t key, int priority=0);

        /** \brief Unregister packet by packet type

            Removes \a PacketType from the packet registry. If the packet type is not registered,
            this is a no-op.

            \tparam PacketType ConcretePacket instantiation of packet to remove from registry
         */
        template <class PacketType>
        static void unregisterPacket();

        /** \brief Unregister packet by key

            Removes the packet registration for \a key (and \a priority) from the registry. If no
            packet is registered with the given pair \a key, \a priority, this operation is a
            no-op.

            \param[in] key Key to remove from the registry
            \param[in] priority Optional priority of the key to remove
         */
        static void unregisterPacket(typename Tag::key_t key, int priority=0);

        /** \brief Find key of a packet type

            Return the key of \a PacketType as registered in the \a Tag registry

            \tparam PacketType packet of which the key is requested
            \returns key of the packet
            \throws PacketTypeNotRegistered if the packet type is not found in the registry.
         */
        template <class PacketType>
        static typename Tag::key_t key();

        /** \brief Find key of a packet type

            Return the key of \a PacketType as registered in the \a Tag registry

            \tparam PacketType packet of which the key is requested
            \returns key of the packet wrapped in a <a
                href="http://www.boost.org/libs/optional/doc/optional.html">boost::optional</a> or
                an unbound optional, if the key is not found.
         */
        template <class PacketType>
        static typename boost::optional<typename Tag::key_t> key(NoThrow_t);

        /** \brief Find key of a packet

            Return the key of \a packet, an arbitrary packet, as registered in the \a Tag registry.

            \param packet The packet of which the key is requested
            \returns key of the packet
            \throws PacketTypeNotRegistered if the packet type is not found in the registry.
         */
        static typename Tag::key_t key(Packet const & packet);

        /** \brief Find key of a packet

            Return the key of \a packet, an arbitrary packet, as registered in the \a Tag registry.

            \param packet The packet of which the key is requested
            \returns key of the packet wrapped in a <a
                href="http://www.boost.org/libs/optional/doc/optional.html">boost::optional</a> or
                an unbound optional, if the key is not found.
         */
        static typename boost::optional<typename Tag::key_t> key(Packet const & packet, NoThrow_t);

        /** \brief Lookup a packet by it's key

            \throws PacketTypeNotRegistered if the \a key is not found in the registry
            \return %Packet entry for given \a key
         */
        static Entry const & lookup(typename Tag::key_t key);

        /** \brief Lookup a packet by it's key
            \return Pointer to packet entry for given \a key or 0, if the key is not found in the
                registry.
         */
        static Entry const * lookup(typename Tag::key_t key, NoThrow_t);

        /** \brief Beginning iterator to list of registered entries
         */
        static iterator begin();

        /** \brief End iterator to list of registered entries
         */
        static iterator end();

    private:
        typedef detail::PacketRegistryImpl<typename Tag::key_t> Registry;
        static Registry & registry();
    };

    /** \brief Statically add an entry to a packet registry

        This macro will declare an anonymous global variable in such a way, that constructing this
        variable will add a registration to the given packet registry.

        \ingroup packet_module
        \hideinitializer
     */
#   define SENF_PACKET_REGISTRY_REGISTER( registry, value, type )       \
        namespace {                                                     \
            senf::PacketRegistry< registry >::ScopedRegistrationProxy< type > \
            BOOST_PP_CAT(packetRegistration_, __LINE__) ( value );      \
        }

    /** \brief Statically add an entry to a packet registry with explicit priority

        This macro will declare an anonymous global variable in such a way, that constructing this
        variable will add a registration to the given packet registry.

        \ingroup packet_module
        \hideinitializer
     */
#   define SENF_PACKET_REGISTRY_REGISTER_PRIORITY( registry, value, priority, type ) \
        namespace {                                                     \
            senf::PacketRegistry< registry >::ScopedRegistrationProxy< type > \
            BOOST_PP_CAT(packetRegistration_, __LINE__) ( value, priority ); \
        }

    /** \brief Dump all packet registries

        This command will dump all packet registries to the given stream. This is to help debugging
        registration problems.
     */
    void dumpPacketRegistries(std::ostream & os);

    /** \brief Entry not found in registry

        This exception is signaled whenever a throwing lookup operation fails.
     */
    struct PacketTypeNotRegisteredException : public senf::Exception
    { PacketTypeNotRegisteredException() : senf::Exception("packet type not registered") {} };

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketRegistry_i_)
#define HH_SENF_Packets_PacketRegistry_i_
#include "PacketRegistry.cci"
#include "PacketRegistry.ct"
#include "PacketRegistry.cti"
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
