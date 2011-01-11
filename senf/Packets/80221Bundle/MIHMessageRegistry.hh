// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MIH Message-Registry public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHMessageRegistry_
#define HH_SENF_Packets_80221Bundle_MIHMessageRegistry_ 1

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include <senf/Utils/singleton.hh>
#include <senf/Packets/Packets.hh>

//#include "MIHMessageRegistry.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    namespace detail {

        template<class T, typename Signature>
        struct has_static_validate_member
        {
            template<Signature *>
            struct helper;

            template<class U>
            static char test(helper<&U::validate> *);

            template<class U>
            static char (&test(...))[2];

            static const bool value = sizeof(test<T>(0))==1;
        };

        struct MIHMessageRegistry_EntryBase {
            virtual ~MIHMessageRegistry_EntryBase() {}
            virtual void validate(senf::Packet message) const = 0;
        };

        template <class MIHPacket,
            bool use_validate_member = has_static_validate_member<typename MIHPacket::type, void(MIHPacket)>::value>
        struct MIHMessageRegistryEntry : MIHMessageRegistry_EntryBase
        {
            virtual void validate(senf::Packet message) const {}
        };

        template <class MIHPacket>
        struct MIHMessageRegistryEntry<MIHPacket, true> : MIHMessageRegistry_EntryBase
        {
            virtual void validate(senf::Packet message) const {
                MIHPacket::type::validate(message.as<MIHPacket>());
            }
        };
    }


    class MIHMessageRegistry
        : public senf::singleton<MIHMessageRegistry>
    {
    public:
        typedef boost::uint16_t key_t;

        using senf::singleton<MIHMessageRegistry>::instance;
        friend class senf::singleton<MIHMessageRegistry>;

        template <typename MIHPacket>
        struct RegistrationProxy {
            RegistrationProxy();
        };

        template <typename MIHPacket>
        void registerMessageType();

        void validate(key_t messageId, senf::Packet message);

    private:
        typedef boost::ptr_map<key_t, detail::MIHMessageRegistry_EntryBase > Map;
        Map map_;

        MIHMessageRegistry() {};
    };


#   define SENF_MIH_PACKET_REGISTRY_REGISTER( packet )                      \
        SENF_PACKET_REGISTRY_REGISTER(                                      \
            senf::MIHMessageRegistry, packet::type::MESSAGE_ID, packet )    \
        namespace {                                                         \
            senf::MIHMessageRegistry::RegistrationProxy< packet >           \
            BOOST_PP_CAT(mihPacketRegistration_, __LINE__);                 \
        }

}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MIHMessageRegistry.cci"
#include "MIHMessageRegistry.ct"
//#include "MIHMessageRegistry.cti"
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
