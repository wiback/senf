// $Id$
//
// Copyright (C) 2007
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
    \brief PacketImpl non-inline non-template implementation */

//#include "PacketImpl.ih"

// Custom includes
#include <iterator>
#include "Packets.hh"

//#include "PacketImpl.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::detail::PacketImpl

// This function has a problem being inlined. Somehow, often when calling this, the size of the 
// resulting inlined code would be huge. Need to further debug this.

prefix_ void senf::detail::PacketImpl::release(refcount_t n)
{
    SENF_ASSERT(refcount_ >= n);
    // uah ... we need to be extremely careful here. If refcount_ is n, we want to commit suicide,
    // however the destructor will remove all PacketInterpreters from the list and will thereby
    // decrement refcount -> only decrenebt refcount_ when *not* caling delete
    if (refcount_ == n)
        delete this;
    else
        refcount_ -= n;
}

// interpreter chain

prefix_ void senf::detail::PacketImpl::appendInterpreter(PacketInterpreterBase * p)
{
    interpreters_.push_back(*p);
    p->assignImpl(this);
}

prefix_ void senf::detail::PacketImpl::prependInterpreter(PacketInterpreterBase * p)
{
    interpreters_.push_front(*p);
    p->assignImpl(this);
}

// Data container

prefix_ void senf::detail::PacketImpl::clear(PacketData * self)
{
    PacketInterpreterBase * n (next(static_cast<PacketInterpreterBase*>(self)));
    if (n)
        truncateInterpreters(n);
    iterator first (boost::next(begin(),self->begin_));
    data_.erase(first, boost::next(begin(),self->end_));
    updateIterators(self,self->begin_,-self->size());
}

// private members

prefix_ void senf::detail::PacketImpl::eraseInterpreters(interpreter_list::iterator b,
                                                         interpreter_list::iterator e)
{
    while (b!=e) {
        interpreter_list::iterator i (b++);
        PacketInterpreterBase * p (&(*i));
        interpreters_.erase(i);
        p->releaseImpl(); // might call PacketImpl::release and might delete p
    }
}

prefix_ void senf::detail::PacketImpl::updateIterators(PacketData * self, difference_type pos,
                                                       difference_type n)
{
    // I hate to change the PacketData representation from here, I would have preferred to let
    // PacketData have authority over this but trying that just get's to convoluted so I choose the
    // simple solution and made PacketImpl a friend of PacketData.

    interpreter_list::iterator i (interpreters_.begin());

    // There are three types of packets
    // a) Those which come before 'self' in the interpreter chain
    // b) 'self'
    // c) Those that come afterwards
    // For a), the change must be inside the packet since 'self' must be within those packets
    // For b), the change must also be within since that's the packet we are changeing
    // For c), the change must be outside the packet (we don't allow an upper packet to mess with
    // the the data owned by a packet further down the chain). It can be before or after the
    // packet.

    // a)
    for (; &(*i) != static_cast<PacketInterpreterBase*>(self); ++i) i->end_ += n;

    // b)
    i->end_ += n;

    // c)
    interpreter_list::iterator const i_end (interpreters_.end());
    if (++i != i_end)
        if (pos <= difference_type(i->begin_))
            // pos is before the packet, it must then be before all futher packets ...
            for (; i != i_end; ++i) {
                i->begin_ += n;
                i->end_ += n;
            }
        // else pos is after the packet and we don't need to change anything ...
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "PacketImpl.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
