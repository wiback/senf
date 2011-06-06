// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief Backtrace public header */

#ifndef HH_SENF_Utils_Backtrace_
#define HH_SENF_Utils_Backtrace_ 1

// Custom includes
#include <iostream>

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
