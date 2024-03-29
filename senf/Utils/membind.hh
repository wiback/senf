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
    \brief membind  public header */

/** \defgroup membind Bound Member Functions

    The membind() family of function templates simplifies the creation of simple bound member
    function pointers:

    \code
      struct Foo {
          int test(int x);
      };

      Foo * foo = ...;
      boost::function<int (int)> f = senf::membind(&Foo::test,foo);
      int rv = f(1); // Calls foo->test(1)
    \endcode

    senf::membind() takes either a pointer or an object as second argument. When passing an object,
    <em>that object will be copied into the bound member function returned.</em>

    \idea Make the \a ob argument type an additional P template parameter (using call_traits for the
    exact arg type? Probably we'll get deduction problems then) . The only operation this object
    must support is ob->*fn. This would allow the use of smart pointers. We should keep the T &
    version to still support ob.*fn use.
 */

#ifndef HH_SENF_Utils_membind_
#define HH_SENF_Utils_membind_ 1

// Custom includes
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <senf/config.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \brief Get function pointer

    This macro will get a function pointer of a possibly overloaded function:

    \code
    void foo(int i);
    int foo();

    SENF_FNP(void, foo, (int i)) // Get the address of the first overload
    \endcode

    The macro arguments are the return type, function name and function arguments just as specified
    in the declaration.

    This macro only works for functions at namespace scope or for class static functions. For member
    functions use \ref SENF_MEMFNP() or \ref SENF_MEMBINDFNP().

    \hideinitializer
    \ingroup membind
 */
#define SENF_FNP(ret, fn, args) \
    static_cast<ret (*) args>(& fn)

/** \brief Get function pointer

    This macro will get a member function pointer of a possibly overloaded member function:

    \code
    struct Foo
    {
        void foo(int i);
        int foo() const;
    };

    SENF_MEMFNP(int, Foo, foo, () const) // Get the address of the first overload
    \endcode

    The macro arguments are the return type, class name, function name and function arguments just
    as specified in the declaration.

    This macro only works for member functions. For namespace scope functions or class static
    functions use SENF_FNP.

    This macro returns a member function pointer. To automatically bind this pointer to \c this, use
    \ref SENF_MEMBINDFNP() or use senf::membind() to bind to some other instance besides \c this.

    \hideinitializer
    \ingroup membind
 */
#define SENF_MEMFNP(ret, cls, fn, args) \
    static_cast<ret (cls::*) args>(& cls :: fn)

/** \brief Get function pointer

    This macro will get a member function pointer of a possibly overloaded member function and bind
    it to \c this returning a boost::function object resembling an ordinary non-member function (see
    senf::membind()).

    \code
    struct Foo
    {
        void foo(int i);
        int foo() const;

        Foo()
        {
            // Get bound member function for second overload
            SENF_MEMBINDFNP(int, Foo, foo, () const)
        }

    };
    \endcode

    The macro arguments are the return type, class name, function name and function arguments just
    as specified in the declaration.

    This macro only works for member functions. For namespace scope functions or class static
    functions use SENF_FNP.

    This macro returns a bound member function (a \c boost::function object instance). To get an
    ordinary member function pointer use \ref SENF_MEMFNP(), for non-member function or class static
    functions use \ref MEM_FNP().

    \hideinitializer
    \ingroup membind
 */
#define SENF_MEMBINDFNP(ret, cls, fn, args) \
    senf::membind(SENF_MEMFNP(ret, cls, fn, args), this)

namespace senf {

#define scOBTYPE *
#include <senf/Utils/impl/membind.hh>
#undef scOBTYPE

#define scOBTYPE &
#include <senf/Utils/impl/membind.hh>
#undef scOBTYPE

#ifdef DOXYGEN

    /// \addtogroup membind
    //\{

    /** \brief Build bound member function object

        membind() supports up to 9 function parameters (represented as
        \a Args here). The \a ob argument can be either a pointer or a
        reference to \a T
        \param[in] fn member function pointer
        \param[in] ob object instance to bind this pointer to
        \returns Boost.Function object representing a bound call of \a
            fn on \a ob
     */
    template <typename R, typename T, typename Args>
    boost::function<R (Args)> membind(R (T::* fn)( Args ), T * ob);

    //\}

#endif

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "membind.cci"
//#include "membind.ct"
//#include "membind.cti"
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
