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
#else
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
