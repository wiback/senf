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
    \brief safe_bool public header */

#ifndef HH_SENF_Utils_safe_bool_
#define HH_SENF_Utils_safe_bool_ 1

// Custom includes
#include <senf/autoconf.hh>

//#include "safe_bool.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

#ifdef SENF_CXX11_ENABLED

    template <typename T>
    class comparable_safe_bool
    {
    public:
        explicit operator bool() const;
    };

    template <typename T>
    class safe_bool : public comparable_safe_bool<T> {};

#else

    /** \brief internal safe_bool base class
        \internal

        \see safe_bool
     */
    class safe_bool_base
    {
    protected:
        typedef void (safe_bool_base::*bool_type)() const;
        void this_type_does_not_support_comparisons() const;

        // Just here to make them protected ...

        safe_bool_base();
        safe_bool_base(const safe_bool_base&);
        safe_bool_base& operator=(const safe_bool_base&);
        ~safe_bool_base();
    };

    /** \brief Mixin class for safe bool conversion support (comparable classes)

        \see safe_bool
     */
    template <typename T>
    class comparable_safe_bool
        : public safe_bool_base
    {
    public:
        operator bool_type() const;
        bool operator !() const;

    protected:
        ~comparable_safe_bool();
    };

    /** \brief Mixin class for safe boolean conversion support

        This is a direct yet simplified copy of a safe bool solution by Bjorn Karlsson from
        http://www.artima.com/cppsource/safebool.html

        This mixin provides the client class with safe boolean testing. It is a safe replacement for
        <tt>operator bool</tt>. <tt>operator bool</tt> is problematic since \c bool is an integer
        type. This conversion operator makes the class usable in any numeric context, which can be
        quite dangerous. The <tt>operator void *</tt> solution is much better in this respect but
        still allows two instances of any class having such a <tt>void *</tt> conversion to be
        compared for equality. This again will produce absolutely unexpected results since it will
        not check whether the objects are identical, it will only check, that both return the same
        boolean state.

        This solutions solves all these problems by returning a pointer-to-member which cannot be
        converted to any other type. By providing explicit implementations of \c operator== and \c
        operator!= which fail in an obvious way at compile time, this hazard is removed.

        To make a class boolean testable, just inherit from the mixin and implement \c boolean_test:

        \code
        class Testable
            : public safe_bool<Testable>
        {
        public:
            bool boolean_test() const
            {
                // Perform Boolean logic here
            }
        };

        Testable t = ...;

        if (t) {
           ...
        }
        \endcode

        If the class to be made using senf::safe_bool is itself comparable via it's own \c
        operator==, you must use comparable_safe_bool instead of safe_bool to not loose this
        capability.

        \see comparable_safe_bool
     */
    template <typename T>
    class safe_bool : public comparable_safe_bool<T> {};

    template <typename T, typename U>
    void operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs);

    template <typename T,typename U>
    void operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs);

#endif

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "safe_bool.cci"
//#include "safe_bool.ct"
#include "safe_bool.cti"
//#include "safe_bool.mpp"
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
