// $Id$
//
// Copyright (C) 2007
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
