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
