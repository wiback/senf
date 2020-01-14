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
    \brief InformationElements non-inline non-template implementation */

#include "InformationElements.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANSSIDInfoElementParser            );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANPowerConstraintInfoElementParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANSupportedRatesInfoElementParser  );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANVendorSpecificInfoElementParser  );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANHTCapabilitiesInfoElementParser  );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANMeshIdInfoElementParser          );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANVHTCapabilitiesInfoElementParser );
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANVHTOperationInfoElementParser    );


prefix_ void senf::WLANPowerConstraintInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN PowerConstraint Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  " << unsigned(value()) << "\n";
}

prefix_ void senf::WLANSSIDInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN SSID Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  " << value() << "\n";
}

prefix_ void senf::WLANSupportedRatesInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN SupportedRates Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANVendorSpecificInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN Vendor Specific Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    oui: "    << std::hex << oui() << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANHTCapabilitiesInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN HT Capabilities Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANHTOperationInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN HT Operation Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANMeshIdInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN MeshId Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  " << value() << "\n";
}

prefix_ void senf::WLANVHTCapabilitiesInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN VHT Operation Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

prefix_ void senf::WLANVHTOperationInfoElementParser::dump(std::ostream & os)
    const
{
    os << "  WLAN VHT Operation Information Element\n"
       << "    type:   " << unsigned(type()) << "\n"
       << "    length: " << unsigned(length()) << "\n"
       << "    value:  ToDo!\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
