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

#include "PacketImpl.ih"

// Custom includes
#include <iterator>
#include <map>
#include <string>
#include <boost/format.hpp>
#include <senf/Utils/String.hh>
#include "Packets.hh"

//#include "PacketImpl.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::detail::PacketImpl

prefix_ senf::detail::PacketImpl::~PacketImpl()
{
    // We increment refcount_ to ensure, release() won't call delete again
    ++refcount_;
    eraseInterpreters(interpreters_.begin(), interpreters_.end());
}

prefix_ void senf::detail::PacketImpl::release()
{
    SENF_ASSERT(refcount_ >= 1, "Internal failure: Releasing dead PacketImpl ??");
    // uah ... we need to be extremely careful here. If refcount_ is 1, we want to commit suicide,
    // however the destructor will remove all PacketInterpreters from the list and will thereby
    // decrement refcount -> only decrement refcount_ when *not* calling delete (otherwise
    // the assert above will fail)
    if (refcount_ == 1)
        delete this;
    else
        -- refcount_;
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

prefix_ void senf::detail::PacketImpl::prependInterpreter(PacketInterpreterBase * p,
                                                          PacketInterpreterBase * before)
{
    interpreter_list::iterator i (interpreter_list::current(*before));
    interpreters_.insert(i, *p);
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
    // For b), the change must also be within since that's the packet we are changing
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
            // pos is before the packet, it must then be before all further packets ...
            for (; i != i_end; ++i) {
                i->begin_ += n;
                i->end_ += n;
            }
        // else pos is after the packet and we don't need to change anything ...
}

// Annotations

prefix_ void senf::detail::PacketImpl::dumpAnnotations(std::ostream & os)
{
    for (AnnotationRegistry::iterator i (AnnotationRegistry::instance().begin());
         i != AnnotationRegistry::instance().end(); ++i) {
        void * antn (annotation(*i));
        if (antn)
            AnnotationRegistry::instance().dump(*i, os, antn);
    }
}

prefix_ void * senf::detail::PacketImpl::complexAnnotation(AnnotationRegistry::key_type key)
{
    SENF_ASSERT( key < 0, "complexAnnotation called with invalid key");
    return (ComplexAnnotations::size_type(-key-1) >= complexAnnotations_.size()
            || complexAnnotations_.is_null(-key-1))
        ? 0 : complexAnnotations_[-key-1].get();
}

///////////////////////////////////////////////////////////////////////////
// senf::detail::AnnotationRegistry

prefix_ void senf::detail::AnnotationRegistry::dump(key_type key, std::ostream & os,
                                                    void * annotation)
    const
{
    Registry::const_iterator i (registry_.find(key));
    if (i != registry_.end()) {
        os << fieldName(i->second->v_name());
        i->second->v_dump(os, annotation);
        os << "\n";
    }
}

prefix_ void senf::detail::AnnotationRegistry::dumpRegistrations(std::ostream & os)
{
    boost::format fmt ("%-56.56s  %-4.4s  %-7.7s  %5d\n");
    os << "SENF_PACKET_ANNOTATION_SLOTS = " << SENF_PACKET_ANNOTATION_SLOTS << "\n"
       << "SENF_PACKET_ANNOTATION_SLOTSIZE = " << SENF_PACKET_ANNOTATION_SLOTSIZE << "\n";
    os << fmt % "TYPE" % "FAST" % "COMPLEX" % "SIZE";

    for (Index::const_iterator i (index_.begin()), i_end (index_.end()); i != i_end; ++i) {
        key_type key (i->second);
        std::string nm (i->first);
        if (nm.size() > 56) nm.erase(nm.begin(), nm.begin()+nm.size()-32);
        os << fmt
            % nm
            % (key >= 0 ? "yes" : "no")
            % (isComplex(key) ? "yes" : "no")
            % size(key);
    }
}

prefix_ void senf::dumpPacketAnnotationRegistry(std::ostream & os)
{
    senf::detail::AnnotationRegistry::instance().dumpRegistrations(os);
}

/////////////////////////////cc.e////////////////////////////////////////
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
