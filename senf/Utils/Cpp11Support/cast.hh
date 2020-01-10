//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#ifndef HH_SENF_Utils_Cpp11Support_cast_
#define HH_SENF_Utils_Cpp11Support_cast_ 1

// Custom includes
#include <senf/autoconf.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SENF_CXX11_ENABLED

#include <type_traits>

namespace senf {

    template <typename Enum>
    constexpr typename std::underlying_type<Enum>::type underlying_type_cast(Enum e) {
        return static_cast<typename std::underlying_type<Enum>::type>(e);
    }

}

#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
