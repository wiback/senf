// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

#include "Packet.hh"
#include "Packet.ih"

// Custom includes
#include <boost/utility.hpp> // for next/prior

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// REFERENCE COUNTING
///////////////////////////////////////////////////////////////////////////

// We manage TWO reference counts:
//
// - The first reference counts lives wihin PacketImpl. It counts the
//   number of references to the PacketImpl instance
//
// - The second reference count lives with the Packet interpreters. It
//   counts the number of external references to the Packet facades.
//
// The PacketImpl refcount is always the sum of all it's Packet
// interpreter member refcounts.
//
// The Packet facades are referenced from two sides:
//
// - From the user. These references are counted in the Packet
//   refcount.
//
// - From the list of interpreters. These references are *not*
//   counted.
//
// The Packet facades have an impl_ member. This is set to non-0 only,
// as long as the Packet is a list member. The Packet may be deleted,
// when impl_ == 0 and refcount_ == 0.

// class ListPacketDeleter

// This is the custom deleter used for the pointers in the
// interpreters list. This deleter is only called, when the Packet is
// removed from the interpreters list.
prefix_ void satcom::pkf::impl::ListPacketDeleter::operator()(Packet * p)
{
    PacketImpl * impl = PacketImpl::impl(p);
    if (impl->releaseInterpreter(p))
        delete impl;
}

// struct PacketImpl

prefix_ satcom::pkf::Packet::interpreter_list::iterator
satcom::pkf::impl::PacketImpl::appendInterpreter(Packet * p)
{
    BOOST_ASSERT( p->impl_ == 0 );
    
    this->refcount_ += p->refcount_;
    SATCOM_PKF_REFC_MSG("] PacketImpl::appendInterpreter (" << this << "): refcount_ = " << refcount_ << "\n");
    p->impl_ = this;
    this->interpreters_.push_back(
        boost::shared_ptr<Packet>(p, impl::ListPacketDeleter()));

    p->self_ = boost::prior(this->interpreters_.end());
    return p->self_;
}

prefix_ satcom::pkf::Packet::interpreter_list::iterator
satcom::pkf::impl::PacketImpl::prependInterpreter(Packet * p)
{
    BOOST_ASSERT( p->impl_ == 0 );

    this->refcount_ += p->refcount_;
    SATCOM_PKF_REFC_MSG("] PacketImpl::prependInterpreter (" << this << "): refcount_ = " << refcount_ << "\n");
    p->impl_ = this;
    this->interpreters_.push_front(
        boost::shared_ptr<Packet>(p, pkf::impl::ListPacketDeleter()));

    p->self_ = this->interpreters_.begin();
    return p->self_;
}

// Called, whenever a Packet is removed from the list by the
// ListPacketDeleter;
prefix_ bool satcom::pkf::impl::PacketImpl::releaseInterpreter(Packet * p)
{
    // We have to be extra careful here: This method might be called
    // AFTER the PacketImpl instance has already been destructed while
    // destructing the interpreters list !!
    // If p->refcount_ is > 0 however we know, that this->refcount_
    // must also be > 0 ...
    // So we have to make sure never to access this if p->refcount_==0
    BOOST_ASSERT( p->impl_ == this );
    bool rv (false);
    if (p->refcount_ > 0) {
        this->refcount_ -= p->refcount_;
        rv = !this->refcount_;
        SATCOM_PKF_REFC_MSG("] PacketImpl::releaseInterpreter (" << this << "): refcount_ = " << refcount_ << "\n");
    }
    if (p->unlink())
        delete p;
    return rv;
}

namespace {
    bool whenceCmp(unsigned a, unsigned b, bool end, satcom::pkf::Packet::Whence whence)
    {
        using satcom::pkf::Packet;
        return ((whence == Packet::OUTSIDE && ! end)
                || whence == Packet::BEFORE
                || (whence == Packet::INSIDE && end)) ? a>=b : a>b;
    }
}

prefix_ void
satcom::pkf::impl::PacketImpl::updateIterators(Packet::size_type index,
                                               Packet::difference_type n,
                                               Packet::interpreter_list::iterator self,
                                               Packet::Whence whence)
{
    Packet::interpreter_list::iterator i (interpreters_.begin());
    Packet::interpreter_list::iterator const e (interpreters_.end());
    Packet::Whence w (whence == Packet::AUTO ? Packet::INSIDE : whence);
    for (;i!=e;++i) {
        if (whenceCmp((*i)->end_,index,true,w))
            if (n<0 && (*i)->end_ < index-n)
                (*i)->end_ = index;
            else
                (*i)->end_ += n;
        if (whenceCmp((*i)->begin_,index,false,w))
            if (n<0 && (*i)->begin_ < index-n)
                (*i)->begin_ = index;
            else
                (*i)->begin_ += n;
        if (i == self && whence == Packet::AUTO) w = Packet::OUTSIDE;
        BOOST_ASSERT( (*i)->end_ >= (*i)->begin_ );
    }
}

prefix_ void satcom::pkf::impl::PacketImpl::packet_add_ref(Packet const * p)
{
    p->add_ref();
    if (p->impl_)
        p->impl_->add_ref();
}

prefix_ void satcom::pkf::impl::PacketImpl::packet_release(Packet * p)
{ 
    bool del (p->release());
    if (p->impl_ && p->impl_->release())
        // In this case, del is certainly false here. p might
        // however get deleted now.
        delete p->impl_;
    if (del)
        delete p;
}

///////////////////////////////////////////////////////////////////////////
// class Packet

prefix_ satcom::pkf::Packet::ptr satcom::pkf::Packet::next()
    const
{
    interpreter_list::iterator n = boost::next(this->self_);
    if (n == this->impl_->interpreters_.end()) {
        if (this->parsed_)
            return ptr(0);
        // FIXME: v_nextInterpreter return bool? new Interpreter to be
        // added ? hmm ... this however is quite suboptimal ...
        this->v_nextInterpreter();
        this->parsed_ = true;
        n = boost::next(this->self_);
        if (n == this->impl_->interpreters_.end())
            return ptr(0);
    }
    // Re-converting the Packet to a smart pointer is correct here,
    // since the shared_ptr really uses the intrusive refcount which
    // makes this operation safe ...
    return ptr(n->get(),true);
}

prefix_ satcom::pkf::Packet::ptr satcom::pkf::Packet::last()
    const
{
    Packet * p = this->impl_->interpreters_.back().get();
    while (! p->parsed_) {
        Packet * pp = p->next().get();
        if (pp) p = pp;
    }
    // Re-converting the to a smart pointer is correct here, since the
    // shared_ptr really uses the intrusive refcount which makes this
    // operation safe ...
    return ptr(p,true);
}

prefix_ void satcom::pkf::Packet::i_registerInterpreter(Packet * p)
    const
{
    BOOST_ASSERT( !p->impl_ );
    this->impl_->truncateInterpretersAfter(this);
    this->impl_->appendInterpreter(p);
    this->parsed_ = true;
}

prefix_ void satcom::pkf::Packet::i_replaceInterpreter(Packet * p)
{
    BOOST_ASSERT( !p->impl_ );
    // We need to increment the refcount of impl_ beforehand,
    // otherwise it might get deleted by the truncateInterpreters call
    boost::intrusive_ptr<impl::PacketImpl> impl (this->impl_,true);
    impl->truncateInterpreters(this);
    impl->appendInterpreter(p);
}

prefix_ void satcom::pkf::Packet::i_setInterpreter(impl::PacketImpl * i)
{
    // Using prependInterpreter makes this usable for both, the
    // create-from-data and wrap-packet constructors
    i->prependInterpreter(this);
}

prefix_ void satcom::pkf::Packet::insert(iterator pos, byte v, Whence whence)
{
    size_type index(pos-impl_->data_.begin());
    BOOST_ASSERT( index >= begin_ && index <= end_);
    impl_->data_.insert(pos,v);
    impl_->updateIterators(index,1,self_,whence);
}

prefix_ void satcom::pkf::Packet::insert(iterator pos, size_type n, byte v, Whence whence)
{
    size_type index(pos-impl_->data_.begin());
    BOOST_ASSERT( index >= begin_ && index <= end_ );
    impl_->data_.insert(pos,n,v);
    impl_->updateIterators(index,n,self_,whence);
}

prefix_ void satcom::pkf::Packet::erase(iterator pos)
{
    size_type index(pos-impl_->data_.begin());
    BOOST_ASSERT( index >= begin_ && index < end_ );
    impl_->data_.erase(pos);
    impl_->updateIterators(index,-1,self_,INSIDE);
}

prefix_ void satcom::pkf::Packet::erase(iterator first, iterator last)
{
    size_type index(first-impl_->data_.begin());
    size_type sz(last-first);
    BOOST_ASSERT( index >= begin_ && index < end_ && sz <= end_-index );
    // FIXME: Here we should assert, that no bytes belonging to the
    // next iterator are deleted ...
    impl_->data_.erase(first,last);
    impl_->updateIterators(index,-sz,self_,INSIDE);
}

prefix_ void satcom::pkf::Packet::dump(std::ostream & os)
    const
{
    v_dump(os);
    ptr p (next());
    if (p)
        p->dump(os);
}

//////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
