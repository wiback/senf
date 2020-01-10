//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
