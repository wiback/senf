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
    \brief Parameters public header */

#ifndef HH_SENF_Utils_Logger_Parameters_
#define HH_SENF_Utils_Logger_Parameters_ 1

// Custom includes

//#include "Parameters.mpp"
#include "Parameters.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

///\addtogroup logging
//\{
///\name Default parameters
//\{

/** \brief Set scope default log stream
    \hideinitializer
 */
#define SENF_LOG_DEFAULT_STREAM(stream) typedef stream SENFLogDefaultStream

/** \brief Set scope default log area
    \hideinitializer
 */
#define SENF_LOG_DEFAULT_AREA(area) typedef area SENFLogDefaultArea

/** \brief Set scope default log level
    \hideinitializer
 */
#define SENF_LOG_DEFAULT_LEVEL(level) typedef level SENFLogDefaultLevel

//\}
//\}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Parameters.cci"
//#include "Parameters.ct"
//#include "Parameters.cti"
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
