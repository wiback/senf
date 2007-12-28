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
    \brief main.test public header */

#ifndef HH_main_test_
#define HH_main_test_ 1

// Custom includes

#ifdef SENF_LOG_CONF
#undef SENF_LOG_CONF
#endif

#define SENF_LOG_CONF (( (senf)(log)(Debug), (_), NOTICE )) \
                      (( (senf)(log)(test)(myStream), (senf)(log)(test)(Foo), VERBOSE ))

#include "Logger.hh"

//#include "main.test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {
namespace test {
    
    struct Foo
    {
        SENF_LOG_CLASS_AREA();

        static void log() {
            SENF_LOG(("Foo::log"));
        }
    };

    SENF_LOG_DEF_ALIAS( LogCritical, (senf::log::Debug) (senf::log::CRITICAL) );
    SENF_LOG_DEF_STREAM( myStream, senf::log::MESSAGE, senf::log::MESSAGE, senf::log::MESSAGE );
    SENF_LOG_DEF_AREA( myArea );

}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "main.test.cci"
//#include "main.test.ct"
//#include "main.test.cti"
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
