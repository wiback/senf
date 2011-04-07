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
    \brief Backtrace non-inline non-template implementation */

#include "Backtrace.hh"
//#include "Backtrace.ih"

// Custom includes
#include <senf/config.hh>
#ifdef SENF_BACKTRACE
    #include <execinfo.h>
#endif
#include <cxxabi.h>
#include <boost/regex.hpp>
#include "Buffer.hh"

//#include "Backtrace.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::formatBacktrace(std::ostream & os, void ** backtrace, unsigned numEntries)
{
#ifdef SENF_BACKTRACE
    char ** symbols (::backtrace_symbols(backtrace, numEntries));

    static boost::regex const backtraceRx
        ("(.*)\\((.*)\\+(0x[0-9a-f]+)\\) \\[(0x[0-9a-f]+)\\]");
    enum { File = 1,
           Symbol = 2,
           Offset = 3,
           Address = 4 };

    for (unsigned i=0; i<numEntries; ++i) {
        std::string sym (symbols[i]);
        boost::smatch match;
        if (regex_match(sym, match, backtraceRx)) {
            std::string symbol (match[Symbol]);
            int status (0);
            char * demangled ( abi::__cxa_demangle(symbol.c_str(), 0, 0, &status) );
            if (demangled) {
                symbol = std::string(demangled);
                free(demangled);
            }
            os << "    " << symbol << " + " << match[Offset]
               << "\n        in " << match[File] << " [" << match[Address] << "]\n";
        }
        else if (sym == "[0xffffe410]")
            os << "    __kernel_vsyscall [0xffffe410]\n";
        else if (sym == "[0xffffe420]")
            os << "    __kernel_sigreturn [0xffffe410]\n";
        else if (sym == "[0xffffe440]")
            os << "    __kernel_rt_sigreturn [0xffffe440]\n";
        else
            os << "    " << sym << "\n";
    }
    free(symbols);
#endif
#ifndef SENF_DEBUG
   os << "no backtrace available please compile SENF without final=1\n";
#endif

}

prefix_ void senf::backtrace(std::ostream & os, unsigned numEntries)
{
#ifdef SENF_BACKTRACE
   SENF_SCOPED_BUFFER( void*, entries, numEntries);
   unsigned n ( ::backtrace(entries, numEntries) );
   senf::formatBacktrace(os, entries, n);
#endif
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Backtrace.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
