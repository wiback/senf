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
    \brief Backtrace public header */

#ifndef HH_SENF_Utils_Backtrace_
#define HH_SENF_Utils_Backtrace_ 1

// Custom includes
#include <ostream>

//#include "Backtrace.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
    /** \defgroup backtraces Backtrace formatting and parsing

        These functions help format and parse backtrace information as returned by the glibc
        ::backtrace function.
     */

    /** \brief Format a given backtrace

        This functions will write \a backtrace formatted to \a os. This includes demangling symbol
        names and interpreting some additional kernel symbols.

        \ingroup backtraces
     */
    void formatBacktrace(std::ostream & os, void ** backtrace, int numEntries);

    /** \brief Write a backtrace to \a os

        backtrace() will write a backtrace of the current call stack to \a os.

        \ingroup backtraces
     */
    void backtrace(std::ostream & os, int numEntries);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Backtrace.cci"
//#include "Backtrace.ct"
//#include "Backtrace.cti"
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
