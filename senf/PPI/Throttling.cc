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
    \brief Throttling non-inline non-template implementation */

#include "Throttling.hh"

// Custom includes
#include "Connectors.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::ThresholdThrottling

prefix_ void senf::ppi::ThresholdThrottling::update(connector::GenericPassiveInput & input, Event event)
{
    switch (event) {
    case ENQUEUE:
        if (input.queueSize() >= high_)
            input.throttle();
        break;
    case DEQUEUE:
        if (input.queueSize() <= low_)
            input.unthrottle();
        break;
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
