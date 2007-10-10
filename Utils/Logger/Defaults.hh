// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Defaults public header */

#ifndef HH_Defaults_
#define HH_Defaults_ 1

// Custom includes
#include "Stream.hh"
#include "Area.hh"

//#include "Defaults.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    SENF_LOG_DEF_STREAM(Debug, MESSAGE, DISABLED, DISABLED);
    SENF_LOG_DEF_AREA(DefaultArea);

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Defaults.cci"
//#include "Defaults.ct"
//#include "Defaults.cti"
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
