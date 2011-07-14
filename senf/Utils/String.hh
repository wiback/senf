// $Id$
//
// Copyright (C) 2008
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
