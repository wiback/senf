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
    \brief preprocessor public header */

#ifndef HH_SENF_Utils_preprocessor_
#define HH_SENF_Utils_preprocessor_ 1

// Custom includes
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/dec.hpp>

//#include "preprocessor.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \defgroup senfpp Preprocessor meta programming

    preprocessor.hh provides some additional helper macros based on the Boost.Preprocessor library.
 */

//\{
///\ingroup senfpp

/** \brief Return last element of a sequence
    \hideinitializer
 */
#define SENF_PP_SEQ_BACK(seq) BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(seq)),seq)

# // No recursive call so we need some more of these ... ARGH !!!
# define SENF_CAT_RECURS1(a, b) SENF_CAT_RECURS1_I(a,b)
# define SENF_CAT_RECURS1_I(a, b) a ## b
# define SENF_CAT_RECURS2(a, b) SENF_CAT_RECURS2_I(a,b)
# define SENF_CAT_RECURS2_I(a, b) a ## b
# define SENF_CAT_RECURS3(a, b) SENF_CAT_RECURS3_I(a,b)
# define SENF_CAT_RECURS3_I(a, b) a ## b
#

//\}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "preprocessor.cci"
//#include "preprocessor.ct"
//#include "preprocessor.cti"
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
