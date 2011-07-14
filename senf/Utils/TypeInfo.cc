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
