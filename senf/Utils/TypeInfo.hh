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
