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


#ifndef HH_WiBACK_MIHMessaging_Types_VLanId_
#define HH_WiBACK_MIHMessaging_Types_VLanId_ 1

// Custom includes
#include <set>
#include <unordered_set>
#include <boost/operators.hpp>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>
#include <senf/Utils/Console/Traits.hh>

///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace emu {

    typedef std::uint16_t VLanTag;

    struct VLanId
        : public boost::equality_comparable<VLanId>,
          public boost::less_than_comparable<VLanId>
    {
        static VLanId const None;

        enum Type {NoTag = 0, CTag, STag};

        VLanId(std::uint16_t id = 0, Type type = NoTag);
        VLanId(senf::EthernetPacket const & eth);

        explicit operator bool() const;
        bool operator<(VLanId const & other) const;
        bool operator==(VLanId const & other) const;

        std::uint16_t id() const;
        Type type() const;

        bool ctag() const;
        bool stag() const;
        bool sameTagType(VLanId const & other) const;

        static bool hasSTag(senf::EthernetPacket const & eth);
        static bool hasCTag(senf::EthernetPacket const & eth);
        static bool hasTag(senf::EthernetPacket const & eth);
        static std::uint16_t payloadTypeLength(senf::EthernetPacket const & eth);
        template <class PKT> static PKT payload(senf::EthernetPacket const & eth);
        static Packet payloadPkt(senf::EthernetPacket const & eth);

    private:
        std::uint16_t id_:12;
        std::uint16_t type_:2;

        SENF_CONSOLE_PARSE_FRIEND( VLanId );
    };

    std::size_t hash_value(VLanId const& v);

    typedef std::set<VLanId> VLanIdSet;
    // careful, this (actually the hash() functions) treats None and id() == 0 as same (untagged)
    typedef std::unordered_set<VLanId> VLanIdSetFast;

    std::ostream & operator<<(std::ostream & os, VLanId const & vlanId);

    void senf_console_parse_argument(senf::console::ParseCommandInfo::TokensRange const & tokens, VLanId & vlanId);
}}

namespace std {
    template <>
    struct hash<senf::emu::VLanId> {
        size_t operator () (senf::emu::VLanId const & v) const;
    };
}

///////////////////////////////hh.e////////////////////////////////////////
#include "VLanId.cci"
//#include "VLanId.ct"
//#include "VLanId.cti"
#endif
