// $Id$
//
// Copyright (C) 2007
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
