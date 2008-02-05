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
        
        These are the valid %log levels with some additional special values:

        <dl><dt>VERBOSE</dt><dd>Really verbose %log messages. Messages at this level are used for
        internal debugging. They should be enabled only selectively within the areas currently under
        inspection.</dd>

        <dt>NOTICE</dt><dd>Arbitrary unimportant notice. Message which normally should be disabled
        but might be informative to better understand the programs operation.</dd>

        <dt>MESSAGE</dt><dd>Purely informative message which should be displayed if not explicitly
        disabled.</dd>

        <dt>IMPORTANT</dt><dd>Important information or warning which really should be read.</dd>

        <dt>CRITICAL</dt><dd>Error condition which does not terminate the program completely but has
        non-reversible adverse effects</dd>
        
        <dt>FATAL</dt><dd>Error condition which does terminate program execution or enforces a
        restart or some kind of re-initialization with loss of state and context.</dd></dl>

        There are also some special values which <em>must not be used as a %log level</em>:
        
        <dl><dt>DISABLED</dt><dd>Disable all %log messages.</dd> 

        <dt>NONE</dt><dd>Special value which signifies inheritance of the default %log
        level.</dd></dl>

        Log levels are classes, not numbers. Each %log level class has a \c value member which gives
        that levels numeric priority. The larger the number, the more important the message is.

        \implementation The %log levels need to be classes since areas and streams are classes: Since
            %log arguments (stream, area and level) may appear in any order and number, it is much
            simpler to parse them if they are all of the same type.
     */

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

    /** \brief Log level FATAL
        \see loglevels */
    struct FATAL     : public detail::LevelBase { static unsigned const value = 6; };

    /** \brief Disable logging
        \see loglevels */
    struct DISABLED  : public detail::LevelBase { static unsigned const value = 7; };

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
