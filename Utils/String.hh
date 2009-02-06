// $Id$
//
// Copyright (C) 2008 
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
    \brief String public header */

#ifndef HH_SENF_Utils_String_
#define HH_SENF_Utils_String_ 1

// Custom includes
#include <string>

//#include "String.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /** \brief Join string range with separator into single string

        This utility will build string by joining all elements of \a range into a single string
        using \a sep as separator. The \a range may contain values of any streamable type.
     */
    template <class ForwardReadableRange>
    std::string stringJoin(ForwardReadableRange const & range, std::string sep);

    template <class T>
    std::string str(T const & t);

}

///////////////////////////////hh.e////////////////////////////////////////
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
