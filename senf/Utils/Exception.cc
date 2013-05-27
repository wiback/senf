// $Id$
//
// Copyright (C) 2006
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
    \brief Exception non-inline non-template implementation */

#include "Exception.hh"
//#include "Exception.ih"

// Custom includes
#ifdef SENF_BACKTRACE
   #include <execinfo.h>
#endif
#include <sstream>
#include "Backtrace.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::Exception

prefix_ void senf::ExceptionMixin::addBacktrace()
{
#ifdef SENF_BACKTRACE
    void * entries[SENF_DEBUG_BACKTRACE_NUMCALLERS];
    int nEntries (::backtrace(entries, SENF_DEBUG_BACKTRACE_NUMCALLERS));

    std::stringstream ss;
    ss << "\nException at\n";
    formatBacktrace(ss, entries, nEntries);
    ss << "-- \n";
    excLen_ = ss.str().size();
    ss << what_;
    what_ = ss.str();
#endif
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::Exception

prefix_ senf::Exception::~Exception()
    throw()
{}

prefix_ char const * senf::Exception::what()
    const throw()
{
    return what_.c_str();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
