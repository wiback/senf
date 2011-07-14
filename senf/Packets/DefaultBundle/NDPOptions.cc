// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Sauer <ssauer@berlios.de>


// Definition of non-inline non-template functions

// Custom includes
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
