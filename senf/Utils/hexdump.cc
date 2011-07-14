// $Id$
//
// Copyright (C) 2008
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief hexdump non-inline non-template implementation */

#include "hexdump.hh"
#include "hexdump.ih"

// Custom includes
#include <iomanip>

//#include "hexdump.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN

prefix_ void senf::detail::HexDumper::operator()(unsigned char ch)
{
    if ((offset_ % block_size_) == 0) {
        if (!ascii_.empty()) {
            os_ << "  " << ascii_ << "\n";
            ascii_ = "";
        }
        os_ << "  "
               << std::hex << std::setw(4) << std::setfill('0')  << std::right
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
//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
