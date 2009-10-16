// $Id$
//
// Copyright (C) 2009
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
    \brief InformationElements non-inline non-template implementation */

#include "InformationElements.hh"
//#include "InformationElements.ih"

// Custom includes

//#include "InformationElements.cc.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANInfoElementParser, senf::WLANSSIDInfoElementParser);
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANInfoElementParser, senf::WLANPowerConstraintInfoElementParser);
SENF_PACKET_TLV_REGISTRY_REGISTER( senf::WLANInfoElementParser, senf::WLANSupportedRatesInfoElementParser);


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

///////////////////////////////cc.e////////////////////////////////////////
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
