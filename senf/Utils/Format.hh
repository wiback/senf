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
    \brief Format public header */

#ifndef HH_SENF_Utils_Format_
#define HH_SENF_Utils_Format_ 1

// Custom includes
#include <limits>
#include <ostream>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>

//#include "Format.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        \par \c std::\c setw
            If the width is set >0, the output will be padded internally. If the width is set to
            more than the minimal required output width including internal padding, the output is
            padded on the left or right depending on the streams \c ajustfield setting (changed
            with \c std::left, \c std:;right or \c std::internal). If the \c adjustfield is set to \c
            internal, padding is added between the sign and the number.

        \par \c std::\c setprecision
            The default stream precision is 6. This will schow values with 6 significant digits. The
            count includes the number of digits in front of the decimal point.

        \par \c std::\c showbase, \c std::\c noshowbase
            If the \c showbase flag is set, Instead of writing out the scale exponent in numeric
            form, output the corresponding SI prefix.

        \par \c std::\c showpos, \c std::\c noshowpos
            If the \c showpos flag is set, positive values will have a '+' sign.

        \par \c std::\c showpoint, \c std::\c noshowpoint
            If the \c showpoint flag is set, the exponent will be output even if it is 0. Otherwise,
            if \c width is set, the exponent will be replaced with 4 blanks.

        \par \c std::\c uppercase, \c std::\c nouppercase
            If the \c uppercase flag is set, the exponent letter will be an uppercase 'E' instead of
            'e'. SI prefixes are \e not uppercased, since some SI prefixes differ only in case.

        \par \c std::\c setfill
            The fill character is honored for the outside padding but \e not for the internal
            padding.

        \par \c std::\c left, \c std::\c internal, \c std::\c right
            The alignment flags specify the external padding but do not affect the internal
            padding.

        All these flags may optionally be set by calling members of the senf::format::eng() return
        value with the same name.

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

        senf::str(senf::format::eng(12.345,67).setw().setprecision(5).showpoint().uppercase())
          -> "  12.35+-67.00E+00"
        \endcode

        \param[in] v value
        \param[in] d optional delta

        \ingroup senf_utils_format
     */
    streamable_type eng(float v, float d=NAN);

#else

    class eng
    {
    public:
        eng(float v, float d = std::numeric_limits<float>::quiet_NaN());

        eng const & setw(unsigned w = 1) const;
        eng const & setprecision(unsigned p) const;
        eng const & setfill(char c) const;

        eng const & showbase() const;
        eng const & noshowbase() const;
        eng const & showpos() const;
        eng const & noshowpos() const;
        eng const & showpoint() const;
        eng const & noshowpoint() const;
        eng const & uppercase() const;
        eng const & nouppercase() const;
        eng const & left() const;
        eng const & internal() const;
        eng const & right() const;

    private:
        float v_;
        float d_;

        mutable bool haveWidth_;
        mutable unsigned width_;
        mutable bool havePrecision_;
        mutable unsigned precision_;
        mutable bool haveFill_;
        mutable char fill_;
        mutable std::ios_base::fmtflags mask_;
        mutable std::ios_base::fmtflags flags_;

        friend std::ostream & operator<<(std::ostream & os, eng const & v);

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

    template <class T>
    std::string dumpint(T const & v,
                        typename boost::enable_if<boost::is_signed<typename T::value_type> >::type * = 0);

    template <class T>
    std::string dumpint(T const & v,
                        typename boost::enable_if<boost::is_unsigned<typename T::value_type> >::type * = 0);

#endif

    /** \brief Helper class to easily achieve indent levels

        This class helps to achieve indent levels across function calls. Every instance
        increases the static indent level. On destruction the level is decreased to the level
        before the instance.
        The following example illustrates the use of this class:
        \code
            void f1() {
                senf::format::IndentHelper indent;
                std::cout << indent << "f1\n";
            }
            void f2() {
                senf::format::IndentHelper indent;
                std::cout << indent << "f2 begin\n";
                f1();
                std::cout << indent << "f2 end\n";
            }
            f2()
        \endcode
        Output:
        <pre>
            f2 begin
              f1
            f2 end
        </pre>
        Here <tt>f1()</tt> and <tt>f2()</tt> don't need to know to current indent level,
        they just increase the level by instantiating IndentHelper.

        \ingroup senf_utils_format
     */
    class IndentHelper
    {
        static unsigned int static_level;
        unsigned int instance_level;
    public:

        IndentHelper();                 ///< Construct new IndentHelper instance
                                        /**< The static indent level is increased by one. */
        ~IndentHelper();                ///< Destruct IndentHelper instance
                                        /**< The static indent level will be decreased to the
                                             level before the instance. */
        void increase();                ///< Increase the indent level
                                        /**< The indent level of the instance is increases by one. */
        unsigned int level() const;     ///< return the current indent level
    };

    /** \brief Output indent to given ostream
        \related IndentHelper
     */
    std::ostream & operator<<(std::ostream & os, IndentHelper const & indent);

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
