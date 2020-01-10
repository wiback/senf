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
    \brief String public header */

#ifndef HH_SENF_Utils_String_
#define HH_SENF_Utils_String_ 1

// Custom includes
#include <string>

//#include "String.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Join string range with separator into single string

        This utility will build string by joining all elements of \a range into a single string
        using \a sep as separator. The \a range may contain values of any streamable type.
     */
    template <class ForwardReadableRange>
    std::string stringJoin(ForwardReadableRange const & range, std::string sep);

    /** \brief Get string representation

        Returns the result of streaming the argument into a standard library string-based
        stream and then out as a std::string object. The conversion is done with
        <tt>boost::lexical_cast</tt>. The call is equivalent to
        <pre>
            boost::lexical_cast<std::string>(t);
        </pre>
        \pre
            <tt>t</tt> must be <i>OutputStreamable</i>,
                meaning that an <tt>operator\<\<</tt> is defined\n
            <tt>t</tt> must be <i>CopyConstructible</i>

        \see <a href="http://www.boost.org/doc/libs/release/libs/conversion/lexical_cast.htm">
            boost::lexical_cast</a>
     */
    template <class T>
    std::string str(T const & t);

    template <class Target, class Source>
    Target lexical_cast(Source const & arg);

    namespace detail { template <class Target> class lexical_caster; }

    template <class Target>
    detail::lexical_caster<Target> lexical_cast();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "String.cci"
#include "String.ct"
#include "String.cti"
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
