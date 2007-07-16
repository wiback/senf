// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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


#ifndef HH_PacketRegistryImpl_
#define HH_PacketRegistryImpl_ 1

// Custom includes
#include <map>
#include <boost/utility.hpp> // for boost::noncopyable
#include <boost/optional.hpp>
#include "Utils/Exception.hh"
#include "PacketInterpreter.hh"
#include "Packet.hh"

//#include "PacketRegistry.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct PkReg_Entry 
        : public intrusive_refcount
    {
        virtual ~PkReg_Entry();
        virtual PacketInterpreterBase::factory_t factory() const = 0;
    };

    namespace detail { template <class Key> class PacketRegistryImpl; }

    /** \brief Packet registration facility

        The PacketRegistry provides a generic facility to associate an
        arbitrary key with Packets. Example keys are Ethertype or IP
        protocols.

        Every PacketRegistry is identified by a type tag:
        \code
          struct SomeTag {
              typedef some_key_type key_t;
          };
        \endcode
        The key type can be an arbitrary value type. The PacketRegistry
        for this Tag can then be accessed using
        <code>PacketRegistry<SomeTag>::</code>.

        The PacketRegistry class has only static members and provides
        access to the packet registry. It allows to register Packet
        classes and to create new Packets given a key. Methods are
        also provided to find the key of a Packet type.

        \code
            PacketRegistry<SomeTag>::registerPacket<SomePacket>(key_of_somePacket);
            p = PacketRegistry<SomeTag>::create(some_key,begin,end);
            SomeTag::key_t key = PacketRegistry<SomeTag>::key<SomePacket>();
        \endcode

        Normally, packet classes are registered statically and not
        procedurally. To this end, the RegistrationProxy is provided:
        \code
          PacketRegistry<SomeTag>::RegistrationProxy<SomePacket>
              registerSomePacket (key_of_somePacket);
        \endcode
        This global variable declaration will register \c SomePacket
        with the \c SomeTag registry under the key \c
        key_of_somePacket. The variable \c registerSomePacket is a
        dummy. It's only function is to force the call of it's
        constructor during global construction time.

        The PacketRegistry's purpose is mostly to assist in
        implementing the v_nextInterpreter() member of packet
        facades. This is further supported by the PacketRegistryMixin
        class.

        \todo Add parameterless create() method
     */
    template <class Tag>
    class PacketRegistry
    {
    public:
        /** \brief Statically register a packet type in a PacketRegistry
         */
        template <class PacketType>
        struct RegistrationProxy
        {
            RegistrationProxy(typename Tag::key_t key);
        };

        /** \brief Register new packet type

            Register \c PacketType in the packet registry \c Tag
            under the given \c key.

            \par Preconditions:
                The given \c key must be unique and not be assigned to
                any other packet class in this registry.
                The Packet must not already be registered in the registry.

            \param PacketType packet to register
            \param key key of the packet
         */
        template <class PacketType>
        static void registerPacket(typename Tag::key_t key);

        /** \brief Find key of a packet

            Return the key of \c PacketType as registered in the \c
            Tag registry

            \param PacketType packet of which the key is requested
            \returns key of the packet
            \throws PacketTypeNotRegistered if the packet type is not
                found in the registry.
         */
        template <class PacketType>
        static typename Tag::key_t key();

        template <class PacketType>
        static typename boost::optional<typename Tag::key_t> key(NoThrow_t);

        static typename Tag::key_t key(Packet packet);
        static typename Tag::key_t key(Packet packet, NoThrow_t);

        /** \brief Lookup a packet by it's key

            Returns the packet registration registered under \a key in the \a Tag registry

            \param key Key of the packet registered
            \returns Registration entry of the packet
            \throws PacketTypeNotRegistered if the packet type is not found in the registry
         */
        static PkReg_Entry const & lookup(typename Tag::key_t key);

        static PkReg_Entry const * lookup(typename Tag::key_t key, NoThrow_t);

    private:
        typedef detail::PacketRegistryImpl<typename Tag::key_t> Registry;
        static Registry & registry();
    };

    struct PacketTypeNotRegisteredException : public std::exception
    { virtual char const * what() const throw() { return "packet type not registered"; } };

}

///////////////////////////////hh.e////////////////////////////////////////
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
