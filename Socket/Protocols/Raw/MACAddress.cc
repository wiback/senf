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
    \brief MACAddress non-inline non-template implementation */

#include "MACAddress.hh"
//#include "MACAddress.ih"

// Custom includes
#include <iomanip>
#include <boost/tokenizer.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/range.hpp>

//#include "MACAddress.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    boost::uint8_t hexToNibble(char c)
    {
        if (c<'0')
            throw senf::MACAddress::SyntaxException();
        else if (c<='9')
            return c-'0';
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
    boost::uint8_t hexToByte(Range const & range)
    {
        if (boost::size(range) != 2)
            throw senf::MACAddress::SyntaxException();
        typename boost::range_const_iterator<Range>::type i (boost::begin(range));
        return hexToNibble(i[0])*16+hexToNibble(i[1]);
    }

}

///////////////////////////////////////////////////////////////////////////
// senf::MACAddress

prefix_ senf::MACAddress::MACAddress senf::MACAddress::from_string(std::string const & s)
{
    MACAddress mac (MACAddress::noinit);
    typedef boost::char_separator<char> separator;
    typedef boost::tokenizer<separator> tokenizer;
    separator sep (":-");
    tokenizer tok (s,sep);
    tokenizer::iterator i (tok.begin());
    tokenizer::iterator const i_end (tok.end());
    iterator j (mac.begin());
    iterator const j_end (mac.end());
    for (; i!=i_end && j!=j_end; ++i, ++j)
        *j = hexToByte(*i);
    if (i!=i_end || j!=j_end)
        throw SyntaxException();
    return mac;
}

prefix_ senf::MACAddress senf::MACAddress::from_eui64(boost::uint64_t v)
{
    if ( boost::uint16_t(v>>24)  != 0xfffe )
        throw SyntaxException();
    MACAddress mac (MACAddress::noinit);
    mac[0] = boost::uint8_t( v>>56 );
    mac[1] = boost::uint8_t( v>>48 );
    mac[2] = boost::uint8_t( v>>40 );
    mac[3] = boost::uint8_t( v>>16 );
    mac[4] = boost::uint8_t( v>> 8 );
    mac[5] = boost::uint8_t( v     );
    return mac;
}

///////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::operator<<(std::ostream & os, MACAddress const & mac)
{
    boost::io::ios_all_saver ias(os);
    os << std::hex << std::setfill('0');
    for (MACAddress::const_iterator i (mac.begin()); i != mac.end(); ++i) {
        if (i != mac.begin())
            os << ':';
        os << std::setw(2) << unsigned(*i);
    }
    return os;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "MACAddress.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
