// $Id$
//
// Copyright (C) 2010
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
//   Thorsten Horstmann <tho@berlios.de>

//
/** \file
    \brief QueueingSocketSink non-inline non-template implementation */

#include "QueueingSocketSink.hh"
//#include "QueueingSocketSink.ih"

// Custom includes
#include <senf/Utils/Console/Variables.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PPI_REGISTER_QALGORITHM( "FIFOQueueingAlgorithm", senf::ppi::FIFOQueueingAlgorithm);

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
