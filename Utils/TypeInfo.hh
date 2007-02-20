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
    \brief TypeInfo public header */

#ifndef HH_TypeInfo_
#define HH_TypeInfo_ 1

// Custom includes
#include <string>
#include <typeinfo>

//#include "TypeInfo.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Try to return readable type for given type_info

        This function will try to return a demangled type name for the
        given type_info object. If the demangling fails, the possibly
        mangled name (type->name()) will be returned.

        This function depends on the liberty library provided by the
        linux binutils or binutils-dev packages. It also depends on an
        internal header file. If the API should change, this header
        file (which resides in impl/demangle.h) must be updated from
        the binutils sources.

        \param[in] type type_info object
        \returns type name, possibly demangled
     */
    std::string prettyName(std::type_info const & type);

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "TypeInfo.cci"
//#include "TypeInfo.ct"
//#include "TypeInfo.cti"
//#include "TypeInfo.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
