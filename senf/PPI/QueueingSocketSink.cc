// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
//

/** \file
    \brief QueueingSocketSink non-inline non-template implementation */

#include "QueueingSocketSink.hh"
//#include "QueueingSocketSink.ih"

// Custom includes
#include <senf/Utils/Console/Variables.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PPI_REGISTER_QALGORITHM( "FIFOQueueingAlgorithm", senf::ppi::FIFOQueueingAlgorithm);

///////////////////////////////////////////////////////////////////////////
// senf::ppi::QueueingAlgorithm

prefix_ senf::ppi::QueueingAlgorithm::QueueingAlgorithm()
    : dir_(this)
{
    namespace fty = console::factory;
    dir_.add("size", fty::Command( &QueueingAlgorithm::size, this));
    dir_.add("clear", fty::Command( &QueueingAlgorithm::clear, this));
}

prefix_ senf::console::DirectoryNode & senf::ppi::QueueingAlgorithm::consoleDir()
{
    return dir_;
}

/////////////////////////////////////////////////////////////////////////////
// senf::ppi::QueueingAlgorithmRegistry

prefix_ void senf::ppi::QueueingAlgorithmRegistry::dump(std::ostream & os)
    const
{
    for (QAlgoMap::const_iterator i = qAlgoMap_.begin(); i != qAlgoMap_.end(); ++i) {
        os << i->first << std::endl;
    }
}

prefix_ senf::ppi::QueueingAlgorithm::ptr senf::ppi::QueueingAlgorithmRegistry::createQAlgorithm(std::string const & key)
    const
{
    QAlgoMap::const_iterator i (qAlgoMap_.find( key));
    if (i != qAlgoMap_.end())
        return i->second->create();
    else
        throw Exception("QueueingAlgorithm not registered: ") << key;
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::FIFOQueueingAlgorithm

prefix_ senf::ppi::FIFOQueueingAlgorithm::FIFOQueueingAlgorithm()
    : max_size_( 64)
{
    consoleDir().add("max-size", console::factory::Variable(max_size_) );
}

prefix_ senf::Packet senf::ppi::FIFOQueueingAlgorithm::v_dequeue()
{
    if (queue_.size() > 0) {
        Packet p (queue_.front());
        queue_.pop();
        return p;
    }
    return Packet();
}

prefix_ bool senf::ppi::FIFOQueueingAlgorithm::v_enqueue(Packet const & packet)
{
    if (queue_.size() >= max_size_)
        queue_.pop();
    queue_.push( packet);
    return true;
}

prefix_ void senf::ppi::FIFOQueueingAlgorithm::v_clear()
{
    while (! queue_.empty())
        queue_.pop();
}

prefix_ senf::ppi::QueueingAlgorithm::ptr senf::ppi::FIFOQueueingAlgorithm::create()
{
    return new FIFOQueueingAlgorithm();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
