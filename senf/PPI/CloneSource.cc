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
    \brief CloneSource non-inline non-template implementation */

#include "CloneSource.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ppi::module::CloneSource::CloneSource(senf::Packet const & packet)
    : packet_(packet)
{
    noroute(output);
    output.onRequest(&CloneSource::request);
}

prefix_ void senf::ppi::module::CloneSource::request()
{
    output(packet_.clone());
}

prefix_ void senf::ppi::module::CloneSource::replacePacket(senf::Packet const & packet)
{
    packet_ = packet;
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
