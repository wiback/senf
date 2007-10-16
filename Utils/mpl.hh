// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief mpl public header */

#ifndef HH_mpl_
#define HH_mpl_ 1

// Custom includes
#include "../config.hh"

//#include "mpl.mpp"
#include "mpl.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace mpl {

    /** \defgroup senfmpl Low-level template meta programming helpers
     */

    /** \brief Return-value type used to implement overload selection

        The senf::mpl::rv type is used together with \ref SENF_MPL_RV() to select template
        specializations based on a set of overloads:

        \code
        template <unsigned _> struct select {};

        // Case 0
        template <>
        struct select<0> {
            static bool const has_int_value = true;
            void frobble();
        };
        template <class T>
        senf::mpl::rv<0> select_(int, senf::mpl::take_int<T::value> * = 0);

        // Case 1
        template <>
        struct select<1> {
            static bool const has_int_value = false;
            void dazzle();
        };
        template <class T>
        senf::mpl::rv<1> select_(...);

        template <class T>
        struct choice : public select<SENF_MPL_RV( select_<T>(0) )> {};

        struct A { static const int value = 0; };
        struct B {};

        choice<A> a; a.frobble();
        choice<B> b; b.dazzle();
        \endcode

        The selection is always based on two components: A selector class specialized for each of
        the possible choices and an overloaded function (only signatures, no implementation needed)
        to provide the conditions.

        When instantiatinv <tt>choice<T></tt>, we forward \a T to the <tt>select_</tt> set of
        overloads. Because of <a href="http://en.wikipedia.org/wiki/SFINAE">SFINAE</a>, the overload
        set will only contain those instantiations, for which template expansion does not fail.

        So, if \a T has an integer \c value member, both \c select_ overloads are ok and the call
        <tt>select_<T>(0)</tt> will choose the first (case 0) variant, since the argument \c 0 is
        better matched by \c int than by <tt>...</tt>.

        However, if \a T does not have an integer \c value member, expansion for the first overload
        fails and the overload set only contains the second case.

        \ref SENF_MPL_RV() internally uses \c sizeof to find out, \e which overload was selected
        and returns the senf::mpl::rv-argument of that overloads return type. For this to work, the
        \c select_ functions need not be implemented since no code is generated and \c select_ is
        never called.

        This number is than forwarded as template argument to \c select which is specialized for
        each case. Therefore, <tt>choice<A></tt> has a \c frobble() member whereas
        <tt>choice<B></tt> has a \c dazzle() member.

        \see \ref SENF_MPL_RV
        \ingroup senfmpl
     */
    template <unsigned n>
    struct rv { 
        char _[SENF_MPL_RV_ALIGNMENT][n+1]; 
    };

    /** \brief Get return value of overload selector
        
        Used together with senf::mpl::rv to implement overload selection.
        
        \see \ref senf::mpl::rv
        \ingroup senfmpl
        \hideinitializer
     */
#   define SENF_MPL_RV(expr) (sizeof(expr)/SENF_MPL_RV_ALIGNMENT-1)

    /** \brief Take an arbitrary unsigned integer template argument
        
        Used together with <a href="http://en.wikipedia.org/wiki/SFINAE">SFINAE</a>: The expression
        <tt>take_uint<</tt> \a expr <tt>></tt> is only valid if \a expr is valid and returns a value
        convertible to an unsigned integer.

        \ingroup senfmpl
     */
    template <unsigned long _> struct take_uint {};

    /** \brief Take an arbitrary integer template argument
        
        Used together with <a href="http://en.wikipedia.org/wiki/SFINAE">SFINAE</a>: The expression
        <tt>take_int<</tt> \a expr <tt>></tt> is only valid if \a expr is valid and returns a value
        convertible to an integer.

        \ingroup senfmpl
     */
    template <long _> struct take_int {};

    /** \brief Take an arbitrary type template argument
        
        Used together with <a href="http://en.wikipedia.org/wiki/SFINAE">SFINAE</a>: The expression
        <tt>take_class<</tt> \a expr <tt>></tt> is only valid if \a expr is valid and is a type.

        \ingroup senfmpl
     */
    template <class _> struct take_class {};

    /** \brief Define MPL slot

        The slot macros \ref SENF_MPL_SLOT_DEF(), \ref SENF_MPL_SLOT_SET() and \ref
        SENF_MPL_SLOT_GET() provide a facility to get the last unsigned integer value assigned to
        the slot before the current point in the current class.
        \code
        struct Foo
        {
            // Define SLOT slot named 'accum' initialized to 0
            SENF_MPL_SLOT_DEF(accum, 0);

            // Add 2 to 'accum'
            SENF_MPL_SLOT_SET(accum, SENF_MPL_SLOT_GET(accum) + 2);

            // Multiply 'accum' by 3
            SENF_MPL_SLOT_SET(accum, SENF_MPL_SLOT_GET(accum) * 3);

            // Define the result as a constant expression. result is now 6
            static unsigned result = SENF_MPL_SLOT_GET(accum);
        };
        \endcode
        Of course, it does not make sense to use these macros for simple arithmetic as in the
        example. The SENF_MPL_SLOT macros allow to define macros which pass information from one
        macro invocation to the next.

        \implementation The implementation is based on __LINE__: We check backwards for a value
            defined on a previous line. The check is limited to 80 lines backwards.

        \ingroup senfmpl
        \hideinitializer
     */
#   define SENF_MPL_SLOT_DEF(name,value)                                                          \
        template <class _>                                                                        \
        static senf::mpl::rv<0> _SENF_MPL_SLOT_ ## name (_);                                      \
        SENF_MPL_SLOT_SET(name,value)

    /** \brief Define MPL slot initialized to 0

        This is like \ref SENF_MPL_SLOT_DEF() but initializes the slot to the fixed value 0. The
        advantage over \ref SENF_MPL_SLOT_DEF() is, that this macro may be called from an include
        file whereas all the other \\c SENF_MPL_SLOT_ macros must always be called from the relevant
        file.
     */
#   define SENF_MPL_SLOT_DEF_ZERO(name)                                                           \
        template <class _>                                                                        \
        static senf::mpl::rv<0> _SENF_MPL_SLOT_ ## name (_);

    /** \brief Set MPL slot
        \see \ref SENF_MPL_SLOT_DEF()
        \ingroup senfmpl
        \hideinitializer
     */
#   define SENF_MPL_SLOT_SET(name,value)                                                          \
        static senf::mpl::rv<unsigned(value)+1>                                                   \
            _SENF_MPL_SLOT_ ## name (senf::mpl::take_int<__LINE__>)

    /** \brief Get current MPL slot value
        \see \ref SENF_MPL_SLOT_DEF()
        \ingroup senfmpl
        \hideinitializer
     */
#   define SENF_MPL_SLOT_GET(name)                                                                \
        SENF_MPL_SLOT_I_GET(name)

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "mpl.cci"
//#include "mpl.ct"
//#include "mpl.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
