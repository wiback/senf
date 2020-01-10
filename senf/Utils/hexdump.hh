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
    \brief public header for hexdump */

#ifndef HH_SENF_Utils_hexdump_
#define HH_SENF_Utils_hexdump_ 1

// Custom includes
#include <ostream>

//#include "hexdump.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Write range [ i, i_end ) to output stream in hexadecimal format */
    template <class Iterator>
    void hexdump(Iterator i, Iterator i_end, std::ostream & stream, unsigned block_size=16);

    /** \brief Return quoted iterator range */
    template <class Iterator>
    std::string hexdumpQuote(Iterator i, Iterator i_end);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "hexdump.cci"
#include "hexdump.ct"
//#include "hexdump.cti"
//#include "hexdump.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
