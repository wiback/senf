// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

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
