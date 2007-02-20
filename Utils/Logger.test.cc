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
    \brief Logger.test unit tests */

//#include "Logger.test.hh"
//#include "Logger.test.ih"

// Custom includes
#include <sstream>
#define _senf_LOG_STREAM logstream
#include "Logger.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

BOOST_AUTO_UNIT_TEST(logger)
{
    std::stringstream logstream;

    SENF_LOG_DEFAULTS( (senf::log::Debug) (senf::log::NOTICE) );
    SENF_LOG_DEF_ALIAS( LogFoo, (senf::log::Debug) (senf::log::CRITICAL) );
    SENF_LOG_DEF_STREAM( myStream );
    SENF_LOG_DEF_AREA( myArea );

    SENF_LOG(("Log message"));

    SENF_LOG((LogFoo) ("Another log message: " << 10));

    SENF_LOG_BLOCK((senf::log::Debug) (senf::log::WARNING) ({
	log << "Last message";
	log << " continued here";
    }));

    BOOST_CHECK_EQUAL( logstream.str(), "Log message\nAnother log message: 10\nLast message continued here\n" );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// ispell-local-dictionary: "american"
// End:
