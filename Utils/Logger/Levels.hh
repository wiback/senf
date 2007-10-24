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
    \brief Levels public header */

#ifndef HH_Levels_
#define HH_Levels_ 1

// Custom includes

//#include "Levels.mpp"
#include "Levels.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    /** \defgroup loglevels Log levels
        
        These are the valid log levels with some additional special values:

        \c DISABLED is a special value used as level limit to disable all messages.

        \c NONE is used to in some special places to inherit the default log level.
     */

    ///\ingroup loglevels
    ///\{
    
    /** \brief Log level VERBOSE
        \see loglevels */
    struct VERBOSE   : public detail::LevelBase { static unsigned const value = 1; };

    /** \brief Log level NOTICE
        \see loglevels */
    struct NOTICE    : public detail::LevelBase { static unsigned const value = 2; };

    /** \brief Log level MESSAGE
        \see loglevels */
    struct MESSAGE   : public detail::LevelBase { static unsigned const value = 3; };

    /** \brief Log level IMPORTANT
        \see loglevels */
    struct IMPORTANT : public detail::LevelBase { static unsigned const value = 4; };

    /** \brief Log level CRITICAL
        \see loglevels */
    struct CRITICAL  : public detail::LevelBase { static unsigned const value = 5; };

    /** \brief Disable logging
        \see loglevels */
    struct DISABLED  : public detail::LevelBase { static unsigned const value = 6; };

    /** \brief Inherit log level
        \see loglevels */
    struct NONE      : public detail::LevelBase { static unsigned const value = 0; };

    ///\}

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Levels.cci"
//#include "Levels.ct"
//#include "Levels.cti"
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
