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


/** \file
    \brief Callback public header */

#ifndef HH_SENF_PPI_detail_Callback_
#define HH_SENF_PPI_detail_Callback_ 1

// Custom includes
#include <boost/function.hpp>

//#include "Callback.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace detail {

    /** \brief Provide callbacks with a single optional argument

        This helper implements callbacks with an optional single argument. In addition to
        boost::function, this helper provides the following functionality:

        \li It allows the callback to ignore the argument: A callable with no argument may be used
            as callback.
        \li It allows to use member function pointers as callbacks. These will be automatically
            bound to the \a owner argument of \ref make().

        The callbacks follow the same restrictions as <a href="">Boost.Function</a>: They must be
        either function, member function pointers or callable objects defining the appropriate
        typedef members.
     */
    template <class Arg=void>
    struct Callback
    {
        typedef boost::function<void (Arg)> type;

        template <class Owner, class FnClass>
        static type make(void (FnClass::* memfn )(), Owner & owner);
        template <class Owner, class FnClass, class FnArg>
        static type make(void (FnClass::* memfn )(FnArg arg), Owner & owner);
        template <class Owner>
        static type make(type callable, Owner &);
        template <class Owner>
        static type make(boost::function<void()> callable, Owner &);
    };

#ifndef DOXYGEN

    template <>
    struct Callback<void>
    {
        typedef boost::function<void ()> type;

        template <class Owner, class FnClass>
        static type make(void (FnClass::* memfn )(), Owner & owner);
        template <class Owner>
        static type make(type callable, Owner &);
    };

#endif

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Callback.cci"
//#include "Callback.ct"
#include "Callback.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u ../test"
// End:
