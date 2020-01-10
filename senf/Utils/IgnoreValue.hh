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
    \brief IgnoreValue public header */

#ifndef HH_SENF_Utils_IgnoreValue_
#define HH_SENF_Utils_IgnoreValue_ 1

// Custom includes

//#include "IgnoreValue.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Explicitly ignore a value

        To explicitly ignore a value (e.g. the return value of a function), pass that value to
        senf::IGNORE(). This has the added benefit of silencing \em any warnings about ignored
        values by g++.
     */
    template <class T>
    void IGNORE(T const &);
    template <class T>
    void IGNORE(T const *);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "IgnoreValue.cci"
//#include "IgnoreValue.ct"
#include "IgnoreValue.cti"
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
