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
    \brief PacketInterpreter non-inline non-template implementation */

//#include "PacketInterpreter.ih"

// Custom includes
#include "Packets.hh"

//#include "PacketInterpreter.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::PacketInterpreterBase

// structors and default members

prefix_  senf::PacketInterpreterBase::~PacketInterpreterBase()
{}

prefix_ senf::PacketInterpreterBase::ptr senf::PacketInterpreterBase::clone()
{
    detail::PacketImpl::Guard p (new detail::PacketImpl(begin(),end()));
    ptr pi (appendClone(p.p,begin(),p.p->begin()));
    for (ptr i (next()); i; i = i->next())
        i->appendClone(p.p,begin(),p.p->begin());
    return pi;
}

// Interpreter chain access

prefix_ senf::PacketInterpreterBase::ptr senf::PacketInterpreterBase::append(ptr packet)
{
    if (next())
        impl().truncateInterpreters(next().get());

    optional_range r (nextPacketRange());
    if (!r)
        throw InvalidPacketChainException();

    ptr rv (packet->appendClone(&impl(), *r));
    rv->data().resize(packet->data().size());
    std::copy(packet->data().begin(), packet->data().end(), rv->data().begin());

    for (ptr p (packet->next()) ; p ; p = p->next())
        p->appendClone(&impl(), packet->data().begin(), rv->data().begin());

    return rv;
}

prefix_ void senf::PacketInterpreterBase::reparse()
{
    if (next())
        impl().truncateInterpreters(next().get());
}

// Access to the abstract interface

prefix_ void senf::PacketInterpreterBase::dump(std::ostream & os)
{
    try {
        if (detail::AnnotationRegistry::instance().begin()
            != detail::AnnotationRegistry::instance().end()) {
            os << "Annotations:\n";
            impl().dumpAnnotations(os);
        }
        v_dump(os);
        for (ptr i (next()); i; i = i->next())
            i->v_dump(os);
    }
    catch (senf::Exception & e) {
        os << "[Exception: " << e.message() << "]\n";
    }
}

prefix_ void senf::PacketInterpreterBase::finalizeThis()
{
    v_finalize();
}

prefix_ void senf::PacketInterpreterBase::finalizeTo(ptr other)
{
    for (ptr i (other); i.get() != this && i.get(); i = i->prev())
        i->finalizeThis();
    finalizeThis();
}

// reference/memory management

prefix_ void senf::PacketInterpreterBase::add_ref()
{
    if (impl_ && !refcount())
        impl_->add_ref();
    intrusive_refcount_t<PacketInterpreterBase>::add_ref();
}

prefix_ void senf::PacketInterpreterBase::release()
{
    if (impl_ && refcount()==1)
        // This call will set impl_ to 0 if we just removed the last reference ...
        impl_->release();
    if (intrusive_refcount_t<PacketInterpreterBase>::release() && !impl_)
        delete this;
}

///////////////////////////////////////////////////////////////////////////
// senf::PacketInterpreterBase::Factory

prefix_  senf::PacketInterpreterBase::Factory::~Factory()
{}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "PacketInterpreter.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
