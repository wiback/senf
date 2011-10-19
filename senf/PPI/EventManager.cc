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
    \brief EventManager non-inline non-template implementation */

#include "EventManager.hh"
//#include "EventManager.ih"

// Custom includes
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

//#include "EventManager.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::EventManager

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

bool senf::ppi::EventManager::alive_ (false);

prefix_ void senf::ppi::EventManager::destroyModule(module::Module & module)
{
    using boost::lambda::_1;
    namespace l = boost::lambda;

    registrations_.erase_if(l::bind(&detail::EventBindingBase::module_,_1) == &module);
}

prefix_ void senf::ppi::EventManager::destroyEvent(EventDescriptor & event)
{
    using boost::lambda::_1;
    namespace l = boost::lambda;

    SENF_ASSERT(
        std::find_if(registrations_.begin(), registrations_.end(),
                     l::bind(&detail::EventBindingBase::descriptor_,_1) == &event)
        == registrations_.end(),
        "Internal failure: registrations still active while destroying event."
        "Maybe you've put the SENF_PPI_MODULE macro in a superclass instead of"
        "in the subclass?");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EventManager.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
