// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#include "EthernetPacket.hh"
//#include "EthernetPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/format.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    namespace pkf = satcom::pkf;
    pkf::PacketRegistry<pkf::EtherTypes>::RegistrationProxy<pkf::EthVLanPacket> 
        registerEthVLanPacket(0x8100);
}

prefix_ void satcom::pkf::EthernetPacket::v_nextInterpreter()
    const
{
    // TODO: Add LLC/SNAP support -> only use the registry
    // for type() values >=1536, otherwise expect an LLC header
    registerInterpreter(type(),begin()+bytes(),end());
}

namespace {
    
    void dumpmac(std::ostream & os, satcom::pkf::EthernetPacket::Parse_MAC mac)
    {
        for (unsigned i = 0; i < 6; ++i) {
            if (i > 0) 
                os << ':';
            os << std::hex << std::setw(2) << std::setfill('0')
               << unsigned(mac[i]);
        }
    }

}

prefix_ void satcom::pkf::EthernetPacket::v_dump(std::ostream & os)
    const
{
    if (type() <= 1500)
        os << "Ethernet 802.3";
    else if (type() >= 0x600)
        os << "Ethernet II (DIX)";
    else 
        os << "Ethernet 802.3 (bad ethertype >1500 and <1536)";
    os << ": \n"
       << "  destination   : ";
    dumpmac(os,destination());
    os << "\n"
       << "  source        : ";
    dumpmac(os,source());
    os << "\n"
       << "  ethertype     : " << std::hex << std::setw(4) << std::setfill('0')
       << unsigned(type()) << "\n" << std::dec;
}

prefix_ void satcom::pkf::EthernetPacket::v_finalize()
{}

prefix_ void satcom::pkf::EthVLanPacket::v_nextInterpreter()
    const
{
    // TODO: Add LLC/SNAP support -> only use the registry
    // for type() values >=1536, otherwise expect an LLC header
    registerInterpreter(type(),begin()+bytes(),end());
}

prefix_ void satcom::pkf::EthVLanPacket::v_finalize()
{}

prefix_ void satcom::pkf::EthVLanPacket::v_dump(std::ostream & os)
    const
{
    os << "Ethernet 802.1q (VLAN):\n"
       << "  priority      : " << priority() << "\n"
       << "  cfi           : " << cfi() << "\n"
       << "  vlan-ID       : " << vlanId() << "\n"
       << "  ethertype     : " << boost::format("%04x") % type() << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
