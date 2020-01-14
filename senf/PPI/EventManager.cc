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
