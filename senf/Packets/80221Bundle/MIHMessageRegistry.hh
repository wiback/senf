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
    \brief MIH Message-Registry public header */

#ifndef HH_SENF_Packets_80221Bundle_MIHMessageRegistry_
#define HH_SENF_Packets_80221Bundle_MIHMessageRegistry_ 1

// Custom includes
#include <boost/ptr_container/ptr_map.hpp>
#include <senf/Utils/singleton.hh>
#include <senf/Packets/Packets.hh>

#include "MIHMessageRegistry.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    class MIHMessageRegistry
        : public senf::singleton<MIHMessageRegistry>
    {
    public:
        typedef boost::uint16_t key_t;

    private:
        MIHMessageRegistry() {};

        typedef boost::ptr_map<key_t, detail::MIHMessageRegistry_EntryBase > Map;
        Map map_;

    public:
        using senf::singleton<MIHMessageRegistry>::instance;
        friend class senf::singleton<MIHMessageRegistry>;

        template <typename MIHPacket>
        struct RegistrationProxy {
            RegistrationProxy(key_t messageId);
        };

        template <typename MIHPacket>
        void registerMessageType(key_t messageId);

        void validate(key_t messageId, senf::Packet message);

        typedef Map::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;
    };


#   define SENF_MIH_PACKET_REGISTRY_REGISTER( packet, messageId )           \
        SENF_PACKET_REGISTRY_REGISTER(                                      \
            senf::MIHMessageRegistry, messageId, packet )                   \
        namespace {                                                         \
            senf::MIHMessageRegistry::RegistrationProxy< packet >           \
            BOOST_PP_CAT(mihPacketRegistration_, __LINE__) (messageId);     \
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
