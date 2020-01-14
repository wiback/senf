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

#ifdef SENF_BACKTRACE

prefix_ void senf::ExceptionMixin::addBacktrace()
{
    void * entries[SENF_DEBUG_BACKTRACE_NUMCALLERS];
    int nEntries (::backtrace(entries, SENF_DEBUG_BACKTRACE_NUMCALLERS));

    std::stringstream ss;
    ss << "\nException at\n";
    formatBacktrace(ss, entries, nEntries);
    ss << "-- \n";
    excLen_ = ss.str().size();
    ss << what_;
    what_ = ss.str();
}

#endif

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
