// $Id$
//
// Copyright (C) 2009
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
    \brief StatisticsTargets public header */

#ifndef HH_SENF_Utils_StatisticsTargets_
#define HH_SENF_Utils_StatisticsTargets_ 1

// Custom includes
#include <string>
#include <senf/Utils/Logger/Log.hh>

//#include "StatisticsTargets.mpp"
#include "StatisticsTargets.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        senf::log::MESSAGE and the area defaults to senf::log::DefaultArea.

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
    unique_or_auto_ptr< unspecified > StatisticsLogger(std::string const & label="")

#else

    unique_or_auto_ptr< detail::StatisticsLogger<void,void,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream>
    unique_or_auto_ptr< detail::StatisticsLogger<Stream,void,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream, class Area>
    unique_or_auto_ptr< detail::StatisticsLogger<Stream,Area,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream, class Area, class Level>
    unique_or_auto_ptr< detail::StatisticsLogger<Stream,Area,Level> > StatisticsLogger(
        std::string const & label="");

#endif

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
