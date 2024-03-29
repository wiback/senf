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
    \brief PacketImpl non-inline non-template implementation */

#include "PacketImpl.ih"

// Custom includes
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <boost/format.hpp>
#include <senf/Utils/String.hh>
#include "Packets.hh"

#ifndef SENF_DISABLE_CONSOLE
#include <senf/Utils/Console.hh>
#endif

//#include "PacketImpl.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::detail::PacketImpl

#ifdef SENF_DEBUG
senf::detail::PacketImpl::size_type senf::detail::PacketImpl::maxPreallocHigh_ (0);
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
senf::detail::PacketImpl::size_type senf::detail::PacketImpl::maxPreallocHeapcount_ (0);
#endif
#endif

#ifndef SENF_DISABLE_CONSOLE

namespace {

    struct ConsoleDirRegistration
    {
        ConsoleDirRegistration()
            {
                namespace fty = senf::console::factory;

                senf::packetConsoleDir()
                    .add("memoryStatus",
                         fty::Command(&ConsoleDirRegistration::memoryStatus));
                senf::packetConsoleDir()
                    .add("dumpAnnotationRegistry",
                         fty::Command(&senf::dumpPacketAnnotationRegistry));
                senf::packetConsoleDir()
                    .add("pktHandleCount",
                         fty::Command(&ConsoleDirRegistration::pktCount));
            }

        static void memoryStatus(std::ostream & os)
            {
                os << "SENF_PACKET_PREALLOC_INTERPRETERS = " << SENF_PACKET_PREALLOC_INTERPRETERS << "\n"
#ifdef SENF_PACKET_NO_HEAP_INTERPRETERS
                   << "SENF_PACKET_NO_HEAP_INTERPRETERS\n"
#endif
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
                   << "SENF_PACKET_ALTERNATIVE_PREALLOC\n"
#endif
#ifdef SENF_DEBUG
                   << "maxPreallocHigh = " << senf::detail::PacketImpl::maxPreallocHigh() << "\n"
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
                   << "maxPreallocHeapcount = " << senf::detail::PacketImpl::maxPreallocHeapcount() << "\n"
#endif
                   << "# of live Packets = " << senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter() << "\n"
#endif
                    ;
            }

        static void pktCount(std::ostream & os)
            {
                os << senf::Packet::pktCount() << std::endl;
            }
        
        static ConsoleDirRegistration instance_;
    };

    ConsoleDirRegistration ConsoleDirRegistration::instance_;

}

#endif

#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS

prefix_ void senf::detail::PacketImpl::destroySelf()
{
    // We increment refcount_ to ensure, release() won't call delete again
    ++refcount_;
    interpreter_list::iterator b (interpreters_.begin());
    interpreter_list::iterator e (interpreters_.end());
    while (b!=e) {
        interpreter_list::iterator i (b ++);
        PacketInterpreterBase * p (&(*i));
        interpreters_.erase(i);
        p->releaseImpl();
        if (preallocHeapcount_ == 0)
            break;
    }
}

#endif

prefix_ bool senf::detail::PacketImpl::release()
{
    SENF_ASSERT(refcount_ >= 1, "Internal failure: Releasing dead PacketImpl ??");
    // uah ... we need to be extremely careful here. If refcount_ is 1, we want to commit suicide,
    // however the destructor will remove all PacketInterpreters from the list and will thereby
    // decrement refcount -> only decrement refcount_ when *not* calling delete (otherwise
    // the assert above will fail)
    if (refcount_ == 1) {
        delete this;
        return true;
    } else {
        -- refcount_;
        return false;
    }
}

// interpreter chain

prefix_ void senf::detail::PacketImpl::memDebug(std::ostream & os)
{
    os << "PacketImpl @" << this << "-" << static_cast<void*>(static_cast<byte*>(static_cast<void*>(this+1))-1)
       << " refcount=" << refcount()
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
       << " preallocHigh=" << preallocHigh_
#endif
#ifndef SENF_PACKET_NO_HEAP_INTERPRETERS
       << " preallocHeapcount=" << preallocHeapcount_
#endif
       << std::endl;
    std::set<void*> used;
    for (interpreter_list::iterator i (interpreters_.begin()), i_end (interpreters_.end());
         i != i_end; ++i)
        used.insert(&(*i));
    std::set<void*> free;
    {
        PreallocSlot * p (preallocFree_);
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
        while (p)
#else
        while (p != prealloc_ + SENF_PACKET_PREALLOC_INTERPRETERS)
#endif
        {
            free.insert(p);
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
            p = p->nextFree_;
#else
            p += p->nextOffset_ + 1;
#endif
        }
    }
    for (unsigned i (0); i <
#ifdef SENF_PACKET_ALTERNATIVE_PREALLOC
             preallocHigh_
#else
             SENF_PACKET_PREALLOC_INTERPRETERS
#endif
             ; ++i) {
        void * p (&(prealloc_[i]));
        if (free.count(p))
            os << "  free @" << p << std::endl;
        else if (! used.count(p))
            os << "  unlinked @" << p
               << " refcount=" << static_cast<PacketInterpreterBase*>(p)->refcount() << std::endl;
    }
}

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
    interpreter_list::iterator i (interpreter_list::s_iterator_to(*before));
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
        p->releaseImpl();
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

prefix_ void senf::detail::PacketImpl::clearAnnotations()
{
    ::memset(simpleAnnotations_, 0, sizeof(simpleAnnotations_));
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    complexAnnotations_.clear();
#endif
}

prefix_ void senf::detail::PacketImpl::assignAnnotations(PacketImpl const & other)
{
    std::copy(&other.simpleAnnotations_[0], &other.simpleAnnotations_[0] +
            sizeof(simpleAnnotations_)/sizeof(simpleAnnotations_[0]), simpleAnnotations_);
#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    complexAnnotations_.assign(
            other.complexAnnotations_.begin(), other.complexAnnotations_.end());
#endif
}

#ifndef SENF_PACKET_NO_COMPLEX_ANNOTATIONS

prefix_ void * senf::detail::PacketImpl::complexAnnotation(AnnotationRegistry::key_type key)
{
    SENF_ASSERT( key < 0, "complexAnnotation called with invalid key");
    return (ComplexAnnotations::size_type(-key-1) >= complexAnnotations_.size()
            || complexAnnotations_.is_null(-key-1))
        ? 0 : complexAnnotations_[-key-1].get();
}

#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
    const
{
    boost::format fmt ("%-56.56s  %-4.4s  %-7.7s  %5d\n");
    os << "SENF_PACKET_ANNOTATION_SLOTS = " << SENF_PACKET_ANNOTATION_SLOTS << "\n"
       << "SENF_PACKET_ANNOTATION_SLOTSIZE = " << SENF_PACKET_ANNOTATION_SLOTSIZE << "\n";
#ifdef SENF_PACKET_NO_COMPLEX_ANNOTATIONS
    os << "SENF_PACKET_NO_COMPLEX_ANNOTATIONS defined\n";
#endif
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
