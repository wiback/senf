//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
    std::unique_ptr< unspecified > StatisticsLogger(std::string const & label="")

#else

    std::unique_ptr< detail::StatisticsLogger<void,void,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream>
    std::unique_ptr< detail::StatisticsLogger<Stream,void,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream, class Area>
    std::unique_ptr< detail::StatisticsLogger<Stream,Area,void> > StatisticsLogger(
        std::string const & label="");

    template <class Stream, class Area, class Level>
    std::unique_ptr< detail::StatisticsLogger<Stream,Area,Level> > StatisticsLogger(
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
