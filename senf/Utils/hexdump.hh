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
