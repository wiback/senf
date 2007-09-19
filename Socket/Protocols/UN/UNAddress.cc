// $Id$
//
// Copyright (C) 2007 
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
    \brief UNAddress non-inline non-template implementation */

#include "UNAddress.hh"
//#include "UNAddress.ih"

// Custom includes

//#include "UNAddress.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::UNAddress::UNAddress()
    //:path("")
{
    path = boost::filesystem::path("");
}

prefix_ senf::UNAddress::UNAddress(boost::filesystem::path p)
    //:path(p)
{
    
}


prefix_ senf::UNAddress::UNAddress senf::UNAddress::fromString(std::string &  s)
{
    return senf::UNAddress::UNAddress(boost::filesystem::path(s));
}

prefix_ senf::UNAddress::UNAddress senf::UNAddress::fromPath(boost::filesystem::path & p){
    return senf::UNAddress::UNAddress(p);
}

prefix_  std::string senf::UNAddress::pathString()
{
    return  path.string();
}


prefix_ const std::ostream & senf::operator<<(std::ostream & os, UNAddress const & addr)
{
    os << addr.pathString();
    return os;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UNAddress.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
