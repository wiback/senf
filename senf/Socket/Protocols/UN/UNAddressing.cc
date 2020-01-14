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
    \brief UNAddressing non-inline non-template implementation */

#include "UNAddressing.hh"
//#include "UNAddressing.ih"

// Custom includes
#include <senf/Socket/Protocols/AddressExceptions.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::UNSocketAddress::path(std::string const & path)
{
    if (path.size() > sizeof(sockaddr_un)-sizeof(short)-1)
        throw AddressSyntaxException() << "UNSocketAddress path too long: " << path;
    socklen(path.size()+sizeof(short));
    strncpy(addr_.sun_path, path.c_str(), socklen()-sizeof(short));
    addr_.sun_path[socklen()-sizeof(short)+1] = 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "UNAddressing.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
