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
    \brief PacketData non-inline non-template implementation */

//#include "PacketData.ih"

// Custom includes
#include "Packets.hh"

//#include "PacketData.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

// Modifying the raw packet data

prefix_ void senf::PacketData::resize(size_type n, byte v)
{
    if (n<size())
        impl().erase(this,boost::next(begin(),n),end());
    else if (n>size())
        impl().insert(this,end(),n-size(),v);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "PacketData.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
