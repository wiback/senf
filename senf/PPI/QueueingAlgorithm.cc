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
    \brief QueueingSocketSink non-inline non-template implementation */

#include "QueueingSocketSink.hh"
//#include "QueueingSocketSink.ih"

// Custom includes
#include <senf/Utils/Console/Variables.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PPI_REGISTER_QALGORITHM( "FIFOQueueingAlgorithm", senf::ppi::FIFOQueueingAlgorithm);
SENF_PPI_REGISTER_QALGORITHM( "NoneQueueingAlgorithm", senf::ppi::NoneQueueingAlgorithm);

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::QueueingAlgorithm

prefix_ senf::ppi::QueueingAlgorithm::QueueingAlgorithm()
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

prefix_ bool senf::ppi::FIFOQueueingAlgorithm::v_enqueue(Packet const & packet, bool force)
{
    if (!force and (queue_.size() >= max_size_))
        return false;
    
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
    return QueueingAlgorithm::ptr(new FIFOQueueingAlgorithm());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
