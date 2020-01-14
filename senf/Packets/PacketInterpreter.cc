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
        impl().truncateInterpreters(nextP());

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
        impl().truncateInterpreters(nextP());
}

// Access to the abstract interface

prefix_ void senf::PacketInterpreterBase::dump(std::ostream & os, DumpPacketAnnotations_t dumpAnnotationsSwitch)
{
    try {
        if (dumpAnnotationsSwitch == dumpAnnotations &&
                detail::AnnotationRegistry::instance().begin() != detail::AnnotationRegistry::instance().end()) {
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

prefix_ void senf::PacketInterpreterBase::memDebug(std::ostream & os)
{
    os << "  interpreter @" << this << " refcount=" << refcount() << std::endl;
    PacketInterpreterBase * n (next().get());
    if (n)
        n->memDebug(os);
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
    SENF_ASSERT(impl_, "Internal failure: release of lone PacketInterpreter");
    if (intrusive_refcount_t<PacketInterpreterBase>::release()) {
        detail::PacketImpl * impl (impl_);
        if (! is_linked())
            releaseImpl(); // This commits suicide !
        // If this is the last handle referencing the Packet, this call will commit suicide
        impl->release();
    }
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
