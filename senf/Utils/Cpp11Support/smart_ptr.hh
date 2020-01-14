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


#ifndef HH_SENF_Utils_Cpp11Support_smart_ptr_
#define HH_SENF_Utils_Cpp11Support_smart_ptr_ 1

// Custom includes
#include <memory>
#include <senf/autoconf.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SENF_CXX11_ENABLED

#include <type_traits>

namespace senf {

    /* stephantl_make_unique.h
       Based on code by Stephan T. Lavavej at
       http://channel9.msdn.com/Series/C9-Lectures-Stephan-T-Lavavej-Core-C-/STLCCSeries6
     */

    namespace detail {

        template<typename T, typename ... Args>
        std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

        template<typename T, typename ... Args>
        std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
            static_assert(std::extent<T>::value == 0,
                    "make_unique<T[N]>() is forbidden, please use make_unique<T[]>(),");
            typedef typename std::remove_extent<T>::type U;
            return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
        }
    }

    template<typename T, typename ... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return detail::make_unique_helper<T>(
            std::is_array<T>(),std::forward<Args>(args)... );
    }
}
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
