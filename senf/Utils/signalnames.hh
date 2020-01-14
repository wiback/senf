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
    \brief signalnames public header */

#ifndef HH_SENF_Utils_signalnames_
#define HH_SENF_Utils_signalnames_ 1

// Custom includes
#include <string>

//#include "signalnames.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Convert signal number to signal name */
    std::string const & signalName(int signal);

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "signalnames.cci"
//#include "signalnames.ct"
//#include "signalnames.cti"
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
