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

/** \file
    \brief EthernetPacket non-inline non-template implementation */

#include "EthernetPacket.hh"
//#include "EthernetPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <boost/tokenizer.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::EthVLanPacket>
        registerEthVLanPacket(0x8100);
}

///////////////////////////////////////////////////////////////////////////
// senf::MACAddress

namespace {
    senf::PacketParserBase::byte hexToNibble(char c)
    {
        if (c<'0')
            throw senf::MACAddress::SyntaxException();
        else if (c<='9')
            return c-'-';
        else if (c<'A')
            throw senf::MACAddress::SyntaxException();
        else if (c<='F')
            return c-'A'+10;
        else if (c<'a')
            throw senf::MACAddress::SyntaxException();
        else if (c<='f')
            return c-'a'+10;
        else
            throw senf::MACAddress::SyntaxException();
    }
    
    template <class Range>
    senf::PacketParserBase::byte hexToByte(Range const & range)
    {
        if (boost::size(range) != 2)
            throw senf::MACAddress::SyntaxException();
        typename boost::range_const_iterator<Range>::type i (boost::begin(range));
        return hexToNibble(i[0])*16+hexToNibble(i[1]);
    }
}

prefix_ senf::MACAddress::MACAddress(std::string addr)
{
    typedef boost::char_separator<char> separator;
    typedef boost::tokenizer<separator> tokenizer;
    separator sep (":");
    tokenizer tok (addr,sep);
    tokenizer::iterator i (tok.begin());
    tokenizer::iterator i_end (tok.end());
    iterator j (begin());
    iterator j_end (end());
    for (; i!=i_end && j!=j_end; ++i, ++j)
        *j = hexToByte(*i);
    if (i!=i_end || j!=j_end)
        throw SyntaxException();
}

///////////////////////////////////////////////////////////////////////////
// senf::EthernetPacketType

namespace {
    void dumpmac(std::ostream & os, senf::MACAddress mac)
    {
        boost::io::ios_all_saver ias(os);
        for (unsigned i = 0; i < 6; ++i) {
            if (i > 0)
                os << ':';
            os << std::hex << std::setw(2) << std::setfill('0')
               << unsigned(mac[i]);
        }
    }
}

prefix_ void senf::EthernetPacketType::dump(packet p, std::ostream & os)
{
    if (p->type() <= 1500)
        os << "Ethernet 802.3";
    else if (p->type() >= 0x600)
        os << "Ethernet II (DIX)";
    else
        os << "Ethernet 802.3 (bad ethertype >1500 and <1536)";
    os << ": \n"
       << "  destination   : ";
    dumpmac(os,p->destination());
    os << "\n"
       << "  source        : ";
    dumpmac(os,p->source());
    os << "\n"
       << "  ethertype     : " << std::hex << std::setw(4) << std::setfill('0')
       << unsigned(p->type()) << "\n" << std::dec;
}

prefix_ void senf::EthVLanPacketType::dump(packet p, std::ostream & os)
{
    os << "Ethernet 802.1q (VLAN):\n"
       << "  priority      : " << p->priority() << "\n"
       << "  cfi           : " << p->cfi() << "\n"
       << "  vlan-ID       : " << p->vlanId() << "\n"
       << "  ethertype     : " << std::hex << std::setw(4) << std::setfill('0')
       << p->type() << "\n" << std::dec;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
