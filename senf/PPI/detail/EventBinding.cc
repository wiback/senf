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
    descriptor_->binding_ = 0;
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
