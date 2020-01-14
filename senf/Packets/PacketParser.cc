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
    \brief PacketParser non-inline non-template implementation */

#include "Packets.hh"
#include "PacketParser.ih"

// Custom includes

//#include "PacketParser.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::PacketParserBase::resize(size_type oldSize, size_type newSize)
{
    safe_data_iterator safe_i (*this);
    if (oldSize > newSize)
        data().erase(i()+newSize, i()+oldSize);
    else if (oldSize < newSize)
        data().insert(i()+oldSize, newSize-oldSize, 0u);
    i_ = safe_i;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "PacketParser.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
