//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EventBinding non-inline non-template implementation */

#include "EventBinding.hh"
//#include "EventBinding.ih"

// Custom includes
#include <senf/PPI/EventManager.hh>
#include <senf/PPI/Module.hh>

//#include "EventBinding.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ppi::detail::EventBindingBase::~EventBindingBase()
{
    descriptor_->enabled(false);
    module().unregisterEvent(*descriptor_);
    descriptor_->binding_ = nullptr;
}

prefix_ void senf::ppi::detail::EventBindingBase::eventTime(ClockService::clock_type time)
{
    // It's hard to make this inline because of a circular header dependency ...
    manager_->eventTime(time);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EventBinding.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u ../test"
// End:
