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
    \brief TypeInfo public header */

#ifndef HH_SENF_Utils_TypeInfo_
#define HH_SENF_Utils_TypeInfo_ 1

// Custom includes
#include <string>
#include <typeinfo>

//#include "TypeInfo.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Try to return readable type for given type_info

        This function will try to return a demangled type name for the given type_info object. If
        the demangling fails, the possibly mangled name (type->name()) will be returned.

        \param[in] type type_info object
        \returns type name, possibly demangled
     */
    std::string prettyName(std::type_info const & type);

    /** \brief Try to return readable type name without namespace or template arguments

        This function will try to return a demangled type name for the given type_info object. If
        the demangling fails, the possibly mangled name (type->name()) will be returned. The
        namespace prefix and template arguments will be stripped.

        \param[in] type type_info object
        \returns type name, possibly demangled and without namespace or template args
     */

    std::string prettyBaseName(std::type_info const & type);

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TypeInfo.cci"
//#include "TypeInfo.ct"
//#include "TypeInfo.cti"
//#include "TypeInfo.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
