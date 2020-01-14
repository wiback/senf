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
    \brief DumpFormat non-inline non-template implementation */

#include "Packets.hh"
//#include "DumpFormat.ih"

// Custom includes
#include <algorithm>
#include <senf/config.hh>

//#include "DumpFormat.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::string senf::fieldName(std::string const & s)
{
    std::string t (std::max(std::string::size_type(SENF_PACKET_DUMP_COLON_COLUMN+1), s.size()+5), ' ');
    std::copy(s.begin(), s.end(), t.begin()+2);
    t[t.size()-2] = ':';
    return t;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "DumpFormat.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
