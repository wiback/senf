// $Id$
//
// Copyright (C) 2010
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
    \brief IgnoreValue public header */

#ifndef HH_SENF_senf_Utils_IgnoreValue_
#define HH_SENF_senf_Utils_IgnoreValue_ 1

// Custom includes

//#include "IgnoreValue.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Explicitly ignore a value

        To explicitly ingore a value (e.g. the return value of a function), pass that value to
        senf::IGNORE(). This has the added benefit of silencing \em any warnings about ignored
        values by g++.
     */
    template <class T>
    void IGNORE(T const &);
    template <class T>
    void IGNORE(T const *);

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IgnoreValue.cci"
//#include "IgnoreValue.ct"
#include "IgnoreValue.cti"
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
