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
    \brief TypeInfo non-inline non-template implementation */

#include "TypeInfo.hh"
//#include "TypeInfo.ih"

// Custom includes
#include <cxxabi.h>
#include <malloc.h>

//#include "TypeInfo.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::string senf::prettyName(std::type_info const & type)
{
    char const * mangled = type.name();
    int status (0);
    char * demangled ( abi::__cxa_demangle(mangled, 0, 0, &status) );
    std::string name (demangled ? demangled : mangled);
    if (demangled)
        ::free(demangled);
    return name;
}

prefix_ std::string senf::prettyBaseName(std::type_info const & type)
{
    std::string name (prettyName(type));
    std::string::size_type e (name.find('<'));
    if (e == std::string::npos) e = name.size();
    std::string::size_type b (name.rfind(':', e));
    if (b == std::string::npos) b = 0; else ++b;
    return name.substr(b,e-b);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "TypeInfo.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
