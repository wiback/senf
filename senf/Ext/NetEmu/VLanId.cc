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


#include "VLanId.hh"

// Custom includes
#include "senf/Utils/Console/Parse.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

senf::emu::VLanId const senf::emu::VLanId::None = senf::emu::VLanId(0x0, NoTag);

prefix_ senf::emu::VLanId::VLanId(std::uint16_t id, Type type)
    : id_(id & 0xfff), type_(type)
{
}

prefix_ std::ostream & senf::emu::operator<<(std::ostream & os, VLanId const & vlanId)
{
    if (vlanId) {
        os << vlanId.id();
        if (vlanId.stag())
            os << "s";
    }
    else {
        os << "None";
    }
    return os;
}

void senf::emu::senf_console_parse_argument(senf::console::ParseCommandInfo::TokensRange const & tokens, VLanId & vlanId)
{
    bool valid (false);
    senf::console::CheckedArgumentIteratorWrapper arg (tokens);
    try {
        VLanId::Type type (VLanId::NoTag);
        std::string targ (tokens[0].value());
        if (targ == "None") {
            vlanId = VLanId::None;
            valid = true;
        } else if (!targ.empty() and targ.back() == 's') {
            targ.pop_back();
            type = VLanId::STag;
        } else {
            type = VLanId::CTag;
        }
        if (type != VLanId::NoTag) {
            std::uint16_t tmp;
            senf::console::parse(std::vector<senf::console::Token> ({senf::console::WordToken(targ)}), tmp);
            if (tmp < 4096) {
                vlanId = VLanId(tmp, type);
                valid = true;
            }
        }
    }
    catch (...) {
    }
    if (valid)
        arg++;  // successfully parsed a VLanId
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
