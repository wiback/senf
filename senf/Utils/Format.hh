// $Id$
//
// Copyright (C) 2009 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief Format public header */

#ifndef HH_SENF_senf_Utils_Format_
#define HH_SENF_senf_Utils_Format_ 1

// Custom includes
#include <limits>
#include <iostream>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>

//#include "Format.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace format {

    /** \defgroup senf_utils_format Formating
     */

#ifdef DOXYGEN

    /** \brief Format value in engineering representation

        The engineering representation is an exponential representation. Exponents however are
        always multiples of 3:
        <pre>
        123.45   -> 123.450e+00
        123.45e2 ->  12.345e+03
        </pre>

        Additionally, an optional delta value may be displayed:
        <pre>
        123.45+-1.34e+03
        </pre>

        senf::format::eng supports several formating options:
        \par \c std::setw()
            If the width is set >0, the output will be padded internally. If the width is set to
            more than the minimal required output width including internal padding, the output is
            padded on the left or right depending on the streams \c ajustfield setting (changed
            with \c std::left, \c std:;right or \c std::interal). If the \c adjustfield is set to \c
            internal, padding is added between the sign and the number.

        \par \c std::\c setprecision()
            The default stream precision is 6. This will schow values with 6 significant digits. The
            count includes the number of digits in front of the decimal point.

        \par \c std::\c showbase
            If the \c showbase flag is set, Instead of writing out the scale exponent in numeric
            form, output the corresponding SI prefix.

        \par \c std::\c showpos
            If the \c showpos flag is set, positive values will have a '+' sign.

        \par \c std::\c showpoint
            If the \c showpoint flag is set, the exponent will be output even if it is 0. Otherwise,
            if \c width is set, the exponent will be replaced with 4 blanks.
        
        \par \c std::\c uppercase
            If the \c uppercase flag is set, the exponent letter will be an uppercase 'E' instead of
            'e'. SI prefixes are \e not uppercased, since some SI prefixes differ only in case.

        \par \c std::\c setfill()
            The fill character is honored for the outside padding but \e not for the internal
            padding.

        Examples:
        \code
            os << senf::format::eng(1.23);
              -> "1.230"
            os << std::setw(1) << senf::format::eng(1.23);
              -> "   1.230    "
            os << std::setw(25) << std::setprecision(5) << std::showpos << std::uppercase 
               << std::internal << senf::format::eng(12345,67);
              -> "+       12.35+-000.07E+03"
            os << std::showbase << senf::format::eng(12345,67);
              -> "12.345+-0.067k"
        \endcode
        
        \param[in] v value
        \param[in] d optional delta

        \ingroup senf_utils_format
     */
    streamable_type eng(double v, double d=NAN);

#else

    struct eng
    {
        eng(double v_, double d_ = std::numeric_limits<double>::quiet_NaN());

        double v;
        double d;
    };

    std::ostream & operator<<(std::ostream & os, eng const & v);

#endif

#ifdef DOXYGEN

    /** \brief Dump integer value with internal representation

        senf::format::dumpint() will output a signed or unsigned numeric argument in the following
        representations:
        \li hexadecimal notation
        \li decimal notation
        \li byte values interpreted as ASCII characters (in network byte order)

        All fields will be padded depending on the size of the type to a byte boundary (e.g. a 32bit
        integer type will be padded to 8 hex-digits, 10 decimal digits and 4 ASCII characters)

        \ingroup senf_utils_format
     */
    template <class T>
    streamable_type dumpint(T const & v);

#else 

    template <class T> 
    std::string dumpint(T const & v, 
                        typename boost::enable_if<boost::is_signed<T> >::type * = 0);

    template <class T> 
    std::string dumpint(T const & v, 
                        typename boost::enable_if<boost::is_unsigned<T> >::type * = 0);

#endif

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Format.cci"
//#include "Format.ct"
#include "Format.cti"
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
