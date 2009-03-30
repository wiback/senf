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
    \brief StatisticsTargets public header */

#ifndef HH_SENF_PPI_Utils_StatisticsTargets_
#define HH_SENF_PPI_Utils_StatisticsTargets_ 1

// Custom includes
#include <string>
#include "Logger/Logger.hh"

//#include "StatisticsTargets.mpp"
#include "StatisticsTargets.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#ifdef DOXYGEN
    /** \brief Logging stream for statistics messages
        \ingroup senf_statistics
     */
    typedef unspecified StatisticsStream;
#else
    SENF_LOG_DEFINE_STREAM(StatisticsStream, 
                           senf::log::MESSAGE, senf::log::MESSAGE, senf::log::MESSAGE);
#endif

#ifdef DOXYGEN

    /** \brief Send statistics to SENF log

        This statistics target will send all statistics values to the SENF log. The template
        parameters optionally specify the stream, area and/or level to send the messages to. An
        optional \a label string may be specified which will be added as prefix to all log entries.

        The log stream defaults to senf::StatisticsStream, the level defaults to
        senf::log::MESSAGE and the are defaults to senf::log::DefaultArea.

        \code
        // Connect with default parameters
        stats.output(2u).connect()

        // Send statistics messages to the debug stream at default level (MESSAGE)
        // Place them in the MyStatistics area. Add 'stat1' prefix to each log message
        SENF_LOG_DEF_AREA(MyStatistics);
        stats.output(2u).connect<senf::log::Debug, MyStatistics>("stat1");
        \endcode

        \ingroup senf_statistics
     */
    template <class A1=void, class A2=void, class A3=void>
    std::auto_ptr< unspecified > StatisticsLogger(std::string const & label="")

#else

    std::auto_ptr< detail::StatisticsLogger<void,void,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream>
    std::auto_ptr< detail::StatisticsLogger<Stream,void,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream, class Area>
    std::auto_ptr< detail::StatisticsLogger<Stream,Area,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream, class Area, class Level>
    std::auto_ptr< detail::StatisticsLogger<Stream,Area,Level> > StatisticsLogger(
        std::string const & label="");

#endif

}

///////////////////////////////hh.e////////////////////////////////////////
#include "StatisticsTargets.cci"
#include "StatisticsTargets.ct"
#include "StatisticsTargets.cti"
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
