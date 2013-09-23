// $Id$
//
// Copyright (C) 2012
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
