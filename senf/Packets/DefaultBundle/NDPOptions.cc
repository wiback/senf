// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Sauer <ssauer@berlios.de>
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

// Definition of non-inline non-template functions

// Custom includes
#include "NDPOptions.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPSourceLLAddressTLVParser );
    SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPTargetLLAddressTLVParser );
    SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPPrefixInformationTLVParser );
    SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPMTUTLVParser );
}

prefix_ void senf::NDPSourceLLAddressTLVParser::dump(std::ostream & os) const
{
    os << senf::fieldName("    Source Link-layer Address") << "\n";
    os << senf::fieldName("      Link-layer Address")      << source() << "\n";
}

prefix_ void senf::NDPTargetLLAddressTLVParser::dump(std::ostream & os) const
{
    os << senf::fieldName("    Target Link-layer Address") << "\n";
    os << senf::fieldName("      Link-layer Address")      << target() << "\n";
}

prefix_ void senf::NDPPrefixInformationTLVParser::dump(std::ostream & os) const
{
    os << senf::fieldName("    Prefix Information") << "\n";
    os << senf::fieldName("      Prefix Length")                         << unsigned(prefixLength()) << "\n";
    os << senf::fieldName("      On-link Flag")                          << unsigned(l()) << "\n";
    os << senf::fieldName("      Autonomous Address-configuration Flag") << unsigned(a()) << "\n";
    os << senf::fieldName("      Reserved(6Bit)")                        << unsigned(reserved1()) << "\n";
    os << senf::fieldName("      Valid Lifetime")                        << unsigned(validLifetime()) << "\n";
    os << senf::fieldName("      Preferred Lifetime")                    << unsigned(preferredLifetime()) << "\n";
    os << senf::fieldName("      Reserved(32Bit)")                       << unsigned(reserved2()) << "\n";
    os << senf::fieldName("      Prefix")                                << prefix() << "\n";

}

prefix_ void senf::NDPMTUTLVParser::dump(std::ostream & os) const
{
    os << senf::fieldName("    MTU Information") << "\n";
    os << senf::fieldName("      MTU")      << unsigned(mtu()) << "\n";
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
