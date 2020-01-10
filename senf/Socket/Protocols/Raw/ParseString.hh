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
    \brief ParseString public header */

#ifndef HH_SENF_Socket_Protocols_Raw_ParseString_
#define HH_SENF_Socket_Protocols_Raw_ParseString_ 1

// Custom includes

//#include "ParseString.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace detail {

    template <class ForwardIterator>
    void parseHexString(std::string const & value, char const * separator,
                        ForwardIterator f, ForwardIterator l);

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ParseString.cci"
#include "ParseString.ct"
//#include "ParseString.cti"
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
