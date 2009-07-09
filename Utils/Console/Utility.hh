// $Id$
//
// Copyright (C) 2009 
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
    \brief Utility public header */

#ifndef HH_SENF_Utils_Console_Utility_
#define HH_SENF_Utils_Console_Utility_ 1

// Custom includes
#include "Parse.hh"
#include "Traits.hh"

//#include "Utility.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Value range
        
        A value range may be represented in the console either by a single value (setting both \a
        low and \a high to the same value) or as a lower and upper bound seperated by a colon
     */
    template <class T>
    struct ValueRange
    {
        T low;
        T high;
    };

#ifndef DOXYGEN

    template <class T>
    struct ArgumentTraits< ValueRange<T> >
    {
        typedef ValueRange<T> type;
        static bool const singleToken = true;

        static void parse(ParseCommandInfo::TokensRange const & tokens, type & out);
        static std::string description();
        static std::string str(type const & value);
    };

    template <class T>
    struct ReturnValueTraits< ValueRange<T> >
    {
        typedef ValueRange<T> type;

        static void format(type const & value, std::ostream & os);
    };

#endif

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Utility.cci"
#include "Utility.ct"
//#include "Utility.cti"
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
