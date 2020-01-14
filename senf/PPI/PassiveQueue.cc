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
    \brief PassiveQueue non-inline non-template implementation */

#include "PassiveQueue.hh"
//#include "PassiveQueue.ih"

// Custom includes

//#include "PassiveQueue.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::PassiveQueue

prefix_ senf::ppi::module::PassiveQueue::PassiveQueue()
{
    route(input,output);
    input.onRequest(&PassiveQueue::onInput);
    output.onRequest(&PassiveQueue::onOutput);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::module::PassiveQueue::v_init()
{
    if (!input)
        output.throttle();
}

prefix_ void senf::ppi::module::PassiveQueue::onInput()
{
    output.unthrottle();
}

prefix_ void senf::ppi::module::PassiveQueue::onOutput()
{
    if (input)
        output(input());
    if (!input)
        output.throttle();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "PassiveQueue.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
