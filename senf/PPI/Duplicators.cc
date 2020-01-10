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
    \brief Duplicators non-inline non-template implementation */

#include "Duplicators.hh"
//#include "Duplicators.ih"

// Custom includes

//#include "Duplicators.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ppi::module::ActiveDuplicator::ActiveDuplicator()
{
    noroute(input);
    input.onRequest(&ActiveDuplicator::request);
}

prefix_ void senf::ppi::module::ActiveDuplicator::connectorSetup(ConnectorType & conn)
{
    route(input, conn);
}

prefix_ void senf::ppi::module::ActiveDuplicator::request()
{
    Packet p (input());
    ContainerType::iterator i (connectors().begin());
    ContainerType::iterator const i_end (connectors().end());
    for (; i != i_end; ++i)
        (*i)(p);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Duplicators.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
