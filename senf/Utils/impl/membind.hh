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
