// $Id$
//
// Copyright (C) 2006
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

//
// NEVER INCLUDE DIRECTLY !! INCLUDE senf/membind.hh

#if !BOOST_PP_IS_ITERATING

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum_shifted.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>

template <typename R, typename T1, typename T2>
boost::function<R()> membind(R (T1::* fn)(),T2 scOBTYPE ob)
{
    return boost::bind(fn,static_cast<T1 scOBTYPE>(ob));
}

template <typename R, typename T1, typename T2>
boost::function<R()> membind(R (T1::* fn)() const, T2 const scOBTYPE ob)
{
    return boost::bind(fn,static_cast<T1 const scOBTYPE>(ob));
}

template <typename R, typename T1, typename T2>
boost::function<R()> membind(R (T1::* fn)() const, T2 scOBTYPE ob)
{
    return boost::bind(fn,static_cast<T1 const scOBTYPE>(ob));
}

// for BOOST_PP_ITERATION() in 2..9 do
#define BOOST_PP_ITERATION_PARAMS_1 (4, (2, 9, SENF_ABSOLUTE_INCLUDE_PATH(Utils/impl/membind.hh), 1))
#include BOOST_PP_ITERATE()
#else
#if BOOST_PP_ITERATION_DEPTH()==1 && BOOST_PP_ITERATION_FLAGS()==1
// {{

#define scARG(z,n,d) BOOST_PP_CAT(d,n)
#define scPARAMS(d) BOOST_PP_ENUM_SHIFTED(BOOST_PP_ITERATION(),scARG,d)

template < typename R, typename T1, typename T2, scPARAMS(typename A) >
boost::function<R ( scPARAMS(A) )>
membind(R (T1::* fn)( scPARAMS(A) ), T2 scOBTYPE ob)
{
    return boost::bind(fn, static_cast<T1 scOBTYPE>(ob), scPARAMS(_) );
}

template < typename R, typename T1, typename T2, scPARAMS(typename A) >
boost::function<R ( scPARAMS(A) )>
membind(R (T1::* fn)( scPARAMS(A) ) const, T2 const scOBTYPE ob)
{
    return boost::bind(fn, static_cast<T1 const scOBTYPE>(ob), scPARAMS(_) );
}

template < typename R, typename T1, typename T2, scPARAMS(typename A) >
boost::function<R ( scPARAMS(A) )>
membind(R (T1::* fn)( scPARAMS(A) ) const, T2 scOBTYPE ob)
{
    return boost::bind(fn, static_cast<T1 const scOBTYPE>(ob), scPARAMS(_) );
}

#undef scPARAMS
#undef scARG

// }
#endif
#endif
// done


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
