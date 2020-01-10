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
    \brief senflikely public header */

#ifndef HH_SENF_Utils_senflikely_
#define HH_SENF_Utils_senflikely_ 1

// Custom includes

//#include "senflikely.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define SENF_LIKELY(x)      __builtin_expect(!!(x), 1)
#define SENF_UNLIKELY(x)    __builtin_expect(!!(x), 0)


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "senflikely.cci"
//#include "senflikely.ct"
//#include "senflikely.cti"
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
