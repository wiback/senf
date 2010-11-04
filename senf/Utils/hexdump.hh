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
    \brief public header for hexdump */

#ifndef HH_SENF_Utils_hexdump_
#define HH_SENF_Utils_hexdump_ 1

// Custom includes
#include <iostream>

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
