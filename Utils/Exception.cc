// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief Exception non-inline non-template implementation */

#include "Exception.hh"
//#include "Exception.ih"

// Custom includes
#include <cstring>
#include <sstream>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::throwErrno(std::string const & where, int code)
{
    switch (code) {

    // BOOST_PP_REPEAT is limited to 256 repetitions. The max errno value I found in any header file
    // was somewhere around 530 or so. I assume going to 1024 will be good enough. This explicit
    // code will be optimized into a jump table by g++ (which is more efficient than trying to do
    // the table oneself)

#   define ExceptionCase(z, n, data) case n: throw ErrnoException<n>(where);
    BOOST_PP_REPEAT(256, ExceptionCase, _) ;
#   undef ExceptionCase

#   define ExceptionCase(z, n, data) case 256+n: throw ErrnoException<256+n>(where);
    BOOST_PP_REPEAT(256, ExceptionCase, _) ;
#   undef ExceptionCase

#   define ExceptionCase(z, n, data) case 512+n: throw ErrnoException<512+n>(where);
    BOOST_PP_REPEAT(256, ExceptionCase, _) ;
#   undef ExceptionCase

#   define ExceptionCase(z, n, data) case 768+n: throw ErrnoException<768+n>(where);
    BOOST_PP_REPEAT(256, ExceptionCase, _) ;
#   undef ExceptionCase

    default:
        throw SystemException(where, code);
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
