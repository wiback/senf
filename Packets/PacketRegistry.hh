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

#ifndef HH_PacketRegistryImpl_
#define HH_PacketRegistryImpl_ 1

// Custom includes
#include <map>
#include <boost/utility.hpp> // for boost::noncopyable
#include <boost/optional.hpp>
#include "../Utils/Exception.hh"
#include "Packet.hh"

//#include "PacketRegistry.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Registry entry

        Value returned by a registry lookup
     */
    struct PkReg_Entry 
        : public intrusive_refcount
    {
        virtual ~PkReg_Entry();
        virtual Packet::factory_t factory() const = 0;
                                        ///< Get factory of the registered packet type
        virtual std::string name() const = 0;
    };

    namespace detail { template <class Key> class PacketRegistryImpl; }

    /** \brief Packet registration facility

        The PacketRegistry provides a generic facility to associate an arbitrary key with
        Packets. Example keys are Ethertype or IP protocols.

        Every PacketRegistry is identified by a type tag:
        \code
        struct SomeTag {
            typedef some_key_type key_t;
        };
        \endcode
        The key type can be an arbitrary value type. The PacketRegistry for this Tag can then be
        accessed using <code>senf::PacketRegistry<SomeTag>::</code>.

        The PacketRegistry class has only static members and provides access to the packet
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
        senf::PacketRegistry<SomeTag>::RegistrationProxy<SomePacket>
            registerSomePacket (key_of_somePacket);
        \endcode

        This global variable declaration will register \a SomePacket with the \a SomeTag registry
        under the key \a key_of_somePacket. The variable \a registerSomePacket is a dummy. It's only
        function is to force the call of it's constructor during global construction time. This
        static registration only works when the symbol is included into the final binary. To force
        this inclusion, you should not put packet registrations into a library but into an object
        file.
        
        \ingroup packet_module
     */
    template <class Tag>
    class PacketRegistry
    {
    public:
        /** \brief Statically register a packet type in a PacketRegistry

            To use this class, define a global symbol in the following way:
            \code
            namespace {
                senf::PacketRegistry<Tag>::RegistrationProxy<PacketType>
                    registerPacketType (key);
            }
            \endcode Here  \a Tag  is the type  tag of the  registry to  register the packet  in, \a
            PacketType is the packet to register (this  is the ConcretePacket of that packet) and \a
            key is  the key of  type \c Tag::key_t  under which the packet  is to be  registered. \a
            registerPacketType is an arbitrary name for the global symbol.
         */
        template <class PacketType>
        struct RegistrationProxy
        {
            RegistrationProxy(typename Tag::key_t key);
        };

        /** \brief Register new packet type

            Register \a PacketType in the packet registry \a Tag under the given \a key.

            \par Preconditions: The given \a key must be unique and not be assigned to any other
                packet class in this registry.  The Packet must not already be registered in the
                registry.

            \param PacketType ConcretePacket instantiation of packet to register
            \param key The key of the packet
         */
        template <class PacketType>
        static void registerPacket(typename Tag::key_t key);

        /** \brief Find key of a packet type

            Return the key of \a PacketType as registered in the \a Tag registry

            \param PacketType packet of which the key is requested
            \returns key of the packet
            \throws PacketTypeNotRegistered if the packet type is not found in the registry.
         */
        template <class PacketType>
        static typename Tag::key_t key();

        /** \brief Find key of a packet type

            Return the key of \a PacketType as registered in the \a Tag registry

            \param PacketType packet of which the key is requested
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
        static typename Tag::key_t key(Packet packet);

        /** \brief Find key of a packet

            Return the key of \a packet, an arbitrary packet, as registered in the \a Tag registry.

            \param packet The 
packet of which the key is requested
            \returns key of the packet wrapped in a <a
                href="http://www.boost.org/libs/optional/doc/optional.html">boost::optional</a> or
                an unbound optional, if the key is not found.
         */
        static typename boost::optional<typename Tag::key_t> key(Packet packet, NoThrow_t);

        /** \brief Lookup a packet by it's key
            
            \throws PacketTypeNotRegistered if the \a key is not found in the registry
            \return Packet entry for given \a key
         */
        static PkReg_Entry const & lookup(typename Tag::key_t key);

        /** \brief Lookup a packet by it's key
            \return Pointer to packet entry for given \a key or 0, if the key is not found in the
                registry.
         */
        static PkReg_Entry const * lookup(typename Tag::key_t key, NoThrow_t);

    private:
        typedef detail::PacketRegistryImpl<typename Tag::key_t> Registry;
        static Registry & registry();
    };

    /** \brief Statically add an entry to a packet registry

        This macro will declare an anonymous global variable in such a way, that constructing this
        variable will add a registration to the given packet registry.

        \hideinitializer
     */
#   define SENF_PACKET_REGISTRY_REGISTER( registry, value, type )                                       \
        namespace {                                                                                     \
            senf::PacketRegistry< registry >::RegistrationProxy< type >                                 \
                packetRegistration_ ## __LINE__ ( value );                                              \
        }

    /** \brief Entry not found in registry

        This exception is signaled whenever a throwing lookup operation fails.
     */
    struct PacketTypeNotRegisteredException : public std::exception
    { virtual char const * what() const throw() { return "packet type not registered"; } };

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_Packets__decls_) && !defined(HH_PacketRegistryImpl_i_)
#define HH_PacketRegistryImpl_i_
//#include "PacketRegistry.cci"
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

