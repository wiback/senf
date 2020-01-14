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
    \brief StreamRegistry non-inline non-template implementation */

#include "StreamRegistry.hh"
#include "StreamRegistry.ih"

// Custom includes

//#include "Stream.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

unsigned senf::log::detail::StreamBase::nStreams = 0;

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::log::detail::StreamBase

prefix_ senf::log::detail::StreamBase::~StreamBase()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Stream.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
