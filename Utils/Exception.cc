// $Id$
//
// Copyright (C) 2006
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
    \brief Exception non-inline non-template implementation */

#include "Exception.hh"
//#include "Exception.ih"

// Custom includes
#include <execinfo.h>
#include <sstream>
#include "../config.hh"

// Copied from the binutils sources 
#define HAVE_DECL_BASENAME 1
#define HAVE_DECL_ASPRINTF 1
#define HAVE_DECL_VASPRINTF 1
#include "impl/demangle.h"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::Exception

#ifdef SENF_DEBUG
prefix_ void senf::ExceptionMixin::addBacktrace()
{
    void * entries[SENF_DEBUG_BACKTRACE_NUMCALLERS];
    unsigned nEntries( ::backtrace(entries, SENF_DEBUG_BACKTRACE_NUMCALLERS) );
    char ** symbols = ::backtrace_symbols(entries, nEntries);
    
    std::stringstream ss;
    ss << "\nException at\n";
    for (unsigned i=0; i<nEntries; ++i) {
        std::string sym (symbols[i]);
        std::string::size_type fnStart (sym.find("("));
        if (fnStart != std::string::npos) {
            std::string::size_type fnEnd (sym.find(")",fnStart+1));
            if (fnEnd != std::string::npos) {
                std::string fn (sym,fnStart+1, fnEnd-fnStart-1);
                char * demangled ( ::cplus_demangle(fn.c_str(), 
                                                    DMGL_TYPES|DMGL_AUTO) );
                if (demangled) {
                    ss << "  " << demangled << "( ... )" << std::string(sym,fnEnd+1) << "\n";
                    continue;
                }
            }
        }
        ss << "  " << sym << "\n";
    }
    ss << "-- \n" << message_;
    message_ = ss.str();
    free(symbols);
}
#endif

/////////////////////////////////////////////////////////////////////////// 
// senf::Exception

prefix_ senf::Exception::~Exception()
    throw()
{}

prefix_ char const * senf::Exception::what()
    const throw()
{
    return message().c_str();
}

///////////////////////////////////////////////////////////////////////////
// senf::SystemException

prefix_ void senf::SystemException::init(std::string const & descr, int code
                                         _SENF_EXC_DEBUG_ARGS_ND)
{
    code_ = code;
#   ifdef SENF_DEBUG
    if (file && line)
        (*this) << "Exception at " << file << ":" << line << "\n";
#   endif
    (*this) << "[" << errorString() << "]";
    if (! descr.empty()) (*this) << " " << descr;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
