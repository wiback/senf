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
    \brief SenfLog public header */

#ifndef HH_SENF_Utils_Logger_SenfLog_
#define HH_SENF_Utils_Logger_SenfLog_ 1

// Custom includes
#include <senf/config.hh>
#include "Log.hh"

//#include "SenfLog.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    SENF_LOG_DEFINE_STREAM(SenfLog, senf::log::MESSAGE, senf::log::MESSAGE, SENF_SENFLOG_LIMIT);
    SENF_LOG_DEFAULT_STREAM(SenfLog);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "SenfLog.cci"
//#include "SenfLog.ct"
//#include "SenfLog.cti"
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
