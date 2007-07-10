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

prefix_ void senf::SystemException::init()
{
    // We normallyl don't want to consume memory in an exception,
    // however all other solutions to format the message are terribly
    // ugly (since thay must use a static and shared buffer ...)
    std::stringstream s;
    if (where)
        s << where << ": ";
    s << "(" << err << ") " << std::strerror(err);
    buffer_ = s.str();
}

prefix_ char const * senf::SystemException::what()
    const throw()
{
    return buffer_.c_str();
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
