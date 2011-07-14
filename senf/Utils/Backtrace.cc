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
    \brief Backtrace non-inline non-template implementation */

#include "Backtrace.hh"
//#include "Backtrace.ih"

// Custom includes
#include <senf/config.hh>
#ifdef SENF_BACKTRACE
    #include <execinfo.h>
    #include <errno.h>
#endif
#include <cxxabi.h>
#include <boost/regex.hpp>
#include "Buffer.hh"

//#include "Backtrace.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::formatBacktrace(std::ostream & os, void ** backtrace, int numEntries)
{
#ifdef SENF_BACKTRACE
    char ** symbols (::backtrace_symbols(backtrace, numEntries));
    if (symbols == NULL) {
        os << "error on translating backtrace addresses with ::backtrace_symbols: " << std::strerror(errno);
        return;
    }

    static boost::regex const backtraceRx
        ("(.*)\\((.*)\\+(0x[0-9a-f]+)\\) \\[(0x[0-9a-f]+)\\]");
    enum { File = 1,
           Symbol = 2,
           Offset = 3,
           Address = 4 };

    for (int i=0; i<numEntries; ++i) {
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

prefix_ void senf::backtrace(std::ostream & os, int numEntries)
{
#ifdef SENF_BACKTRACE
   SENF_SCOPED_BUFFER( void*, entries, numEntries);
   int n ( ::backtrace(entries, numEntries) );
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
