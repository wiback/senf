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
    \brief Levels public header */

#ifndef HH_SENF_Utils_Logger_Levels_
#define HH_SENF_Utils_Logger_Levels_ 1

// Custom includes

//#include "Levels.mpp"
#include "Levels.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    /** \defgroup loglevels Log levels

        These are the valid %log levels with some additional special values:

        <dl><dt>VERBOSE</dt><dd>Really verbose %log messages. Messages at this level are used for
        internal debugging. They should be enabled only selectively within the areas currently under
        inspection. The default %log stream senf::log::Debug has these messages <em>disabled</em> at
        compile time by default. To enable them, see \ref config.</dd>

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

    //\{

    /** \brief Log level %VERBOSE
        \see loglevels */
    struct VERBOSE   : public detail::LevelBase { static unsigned const value = 1; };

    /** \brief Log level %NOTICE
        \see loglevels */
    struct NOTICE    : public detail::LevelBase { static unsigned const value = 2; };

    /** \brief Log level %MESSAGE
        \see loglevels */
    struct MESSAGE   : public detail::LevelBase { static unsigned const value = 3; };

    /** \brief Log level %IMPORTANT
        \see loglevels */
    struct IMPORTANT : public detail::LevelBase { static unsigned const value = 4; };

    /** \brief Log level %CRITICAL
        \see loglevels */
    struct CRITICAL  : public detail::LevelBase { static unsigned const value = 5; };

    /** \brief Log level %FATAL
        \see loglevels */
    struct FATAL     : public detail::LevelBase { static unsigned const value = 6; };

    /** \brief Disable logging
        \see loglevels */
    struct DISABLED  : public detail::LevelBase { static unsigned const value = 7; };

    /** \brief Inherit %log level
        \see loglevels */
    struct NONE      : public detail::LevelBase { static unsigned const value = 0; };

    //\}

    static char const * const LEVELNAMES[8] = {
        "NONE", "VERBOSE", "NOTICE", "MESSAGE", "IMPORTANT", "CRITICAL", "FATAL", "DISABLED" };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
