// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.de>
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
    \brief IpV6Extensions non-inline non-template implementation */

#include "IpV6Extensions.hh"
//#include "IpV6Extensions.ih"

// Custom includes

//#include "IpV6Extensions.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IpV6Extension_Fragment>
        registerIpV6Extension_Fragment (44);
}

prefix_ void senf::IpV6Extension_Fragment::v_nextInterpreter()
    const
{
    registerInterpreter(nextHeader(),begin()+bytes(),end());
}

prefix_ void senf::IpV6Extension_Fragment::v_finalize()
{}

prefix_ void senf::IpV6Extension_Fragment::v_dump(std::ostream & os)
    const
{
    os << "Internet protocol Version 6 fragment extension:\n"
       << "  next header    : " << unsigned(nextHeader()) << "\n"
       << "  fragment offset: " << std::hex << unsigned(fragmentOffset()) << "\n"
       << "  more fragments : " << (moreFragments()?"yes":"no") << "\n"
       << "  id             : " << std::hex << unsigned(id()) << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IpV6Extensions.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
