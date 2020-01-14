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
    \brief Sysdir public header */

#ifndef HH_SENF_Utils_Console_Sysdir_
#define HH_SENF_Utils_Console_Sysdir_ 1

// Custom includes
#include "Node.hh"

//#include "Sysdir.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    ScopedDirectory<> & sysdir();

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Sysdir.cci"
//#include "Sysdir.ct"
//#include "Sysdir.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
