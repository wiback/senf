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


#include "NDPOptions.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPSourceLLAddressTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPTargetLLAddressTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPPrefixInformationTLVParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::NDPMTUTLVParser );

prefix_ void senf::NDPSourceLLAddressTLVParser::dump(std::ostream & os) const
{
    os << "Source Link-layer Address Option:" << "\n";
    os << senf::fieldName("Link-layer Address") << source() << "\n";
}

prefix_ void senf::NDPTargetLLAddressTLVParser::dump(std::ostream & os) const
{
    os << "Target Link-layer Address Option:" << "\n";
    os << senf::fieldName("Link-layer Address") << target() << "\n";
}

prefix_ void senf::NDPPrefixInformationTLVParser::dump(std::ostream & os) const
{
    os << "Prefix Information Option:" << "\n";
    os << senf::fieldName("Prefix Length")                         << unsigned(prefixLength()) << "\n";
    os << senf::fieldName("On-link Flag")                          << unsigned(l()) << "\n";
    os << senf::fieldName("Autonomous Address-configuration Flag") << unsigned(a()) << "\n";
    os << senf::fieldName("Reserved(6Bit)")                        << unsigned(reserved1()) << "\n";
    os << senf::fieldName("Valid Lifetime")                        << unsigned(validLifetime()) << "\n";
    os << senf::fieldName("Preferred Lifetime")                    << unsigned(preferredLifetime()) << "\n";
    os << senf::fieldName("Reserved(32Bit)")                       << unsigned(reserved2()) << "\n";
    os << senf::fieldName("Prefix")                                << prefix() << "\n";
}

prefix_ void senf::NDPMTUTLVParser::dump(std::ostream & os) const
{
    os << "MTU Information Option:" << "\n";
    os << senf::fieldName("MTU") << unsigned(mtu()) << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
