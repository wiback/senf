// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief hexdump non-inline non-template implementation */

#include "hexdump.hh"
#include "hexdump.ih"

// Custom includes
#include <iomanip>

//#include "hexdump.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
#ifndef DOXYGEN

prefix_ void senf::detail::HexDumper::operator()(unsigned char ch)
{
    if ((offset_ % block_size_) == 0) {
        if (!ascii_.empty()) {
            os_ << "  " << ascii_ << "\n";
            ascii_ = "";
        }
        os_ << "  "
               << std::hex << std::setw(4) << std::setfill('0')
               << offset_ << ' ';
    } else if ((offset_ % block_size_) == block_size_/2) {
        os_ << " ";
        ascii_ += ' ';
    }
    os_ << ' ' << std::hex << std::setw(2) << std::setfill('0')
        << unsigned(ch);
    ascii_ += (ch >= ' ' && ch < 126) ? ch : '.';
    ++ offset_;
}

prefix_ senf::detail::HexDumper::~HexDumper()
{
    if (!ascii_.empty()) {
        for (; (offset_ % block_size_) != 0; ++offset_) {
            if ((offset_ % block_size_) == block_size_/2)
                os_ << " ";
            os_ << "   ";
        }
        os_ << "  " << ascii_ << "\n";
    }
}

#endif
///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "hexdump.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
