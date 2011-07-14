// $Id$
//
// Copyright (C) 2007
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
    \brief PacketInterpreter non-inline non-template implementation */

//#include "PacketInterpreter.ih"

// Custom includes
#include "Packets.hh"

//#include "PacketInterpreter.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
    pi->impl().assignAnnotations( impl());
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::PacketInterpreterBase::Factory

prefix_  senf::PacketInterpreterBase::Factory::~Factory()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
