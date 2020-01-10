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
    \brief PacketVector non-inline non-template implementation */

#include "Packets.hh"
//#include "PacketVector.ih"

// Custom includes

//#include "PacketVector.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

#ifndef SENF_PACKET_STD_CONTAINER

prefix_ senf::PacketVector::iterator senf::PacketVector::moveGrow(iterator pos, size_type n)
{
    // This is called in two cases:
    // a) best insertion side is back but there is not enough tailroom
    // b) a reallocation is needed (neither headroom nor tailroom are sufficient)
    if (b_ >= data_ + n) {
        // there is enough headroom. This can only be true if the best insertion side really is
        // the end but there is not enough tailroom (otherwise moveGrow would not have been called)
        ::memmove(b_ - n, b_, pos - b_);
        b_ -= n;
        return pos - n;
    }
    else
        // otherwise we need a reallocation
        return grow(pos, n);
}

prefix_ senf::PacketVector::iterator senf::PacketVector::grow(iterator pos, size_type n)
{
    size_type posIndex (pos - b_);
    size_type dataSize (e_ - b_);
    size_type requestSize (allocationSize(dataSize + n));
    value_type * newData (static_cast<value_type *>(Pool::ordered_malloc(requestSize >> ChunkSizeIndex)));
    ::memcpy(newData + HeadRoom, b_, posIndex);
    ::memcpy(newData + HeadRoom + posIndex + n, b_ + posIndex, dataSize - posIndex);
    if (owner_)
        Pool::ordered_free(data_, size_ >> ChunkSizeIndex);
    b_ = newData + HeadRoom;
    e_ = b_ + dataSize + n;
    size_ = requestSize;
    data_ = newData;
    owner_ = true;
    return b_ + posIndex;
}

#endif

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "PacketVector.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
