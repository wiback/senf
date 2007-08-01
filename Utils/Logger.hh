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
    \brief Logger public header */

/** \defgroup logger The SENF Logger

    The Loggger infrastructure shall implement a highly flexible compile- and run-time configurable
    logging infrastructure supporting multiple streams, user definable log areas and fine grained
    log levels. Logging can be configured at compile and runtime on any combination of above
    parameters. The library supports a host of log targets and messages can be routed into multiple
    targets at the same time. To allow concise usage of the library, a utility to define logging
    defaults for any scope is provided.

    An important basic concept of the library is, that most of the macros take a variable number of
    arguments. Since this is not supported in the needed manner by the C++ preprocessor, the
    arguments are encoded into a <a
    href="http://www.boost.org/libs/preprocessor/doc/index.html">Boost.Preprocessor</a> like
    sequence:

    \code
    SENF_LOG( (senf::log::Debug)(senf::log::NOTICE)(FroblizerArea)("The log message") );
    \endcode

    The last sequence element always is the log message. Before that we have a number of log
    parameters <i>in arbitrary order</i>. Since giving all the parameters in every log message is
    to verbose, there are two helpful constructs to reduce the verbosity. Using \ref SENF_LOG_DEFAULTS it
    is possible to define the default logging parameters to be used within a given scope. Using
    \ref SENF_LOG_DEF_ALIAS you can define an alias (which is a scoped symbol) as an arbitrary
    combination of parameters.

    \code
    SENF_LOG_DEF_STREAM(userLog);

    class Froblizer
    {
        // Define a new log area
        SENF_LOG_DEF_AREA(FroblizerArea);

        // Set default log parameters for this scope
        SENF_LOG_DEFAULTS( (senf::log::Debug) (senf::log::NOTICE) (FroblizerArea) );

        // Define an alias for emergency messages to the sysadmin.
        // The log area is inherited from the default at the place, where this
        // alias is used *not* where it is defined
        SENF_LOG_DEF_ALIAS(LogEmerg, (userLog) (senf::log::CRITICAL));

        void test();

    public:
        void froblize();
    };

    void Froblizer::froblize()
    {
        SENF_LOG(("This is the Debug stream at level NOTICE in the FroblizeArea"));
        SENF_LOG((senf::log::WARNING) ("Same stream and area but at warning level"));
        SENF_LOG((LogEmerg) ("This goes to the userLog at level CRITICAL in the FroblizerArea"));
    }

    void Froblizer::test()
    {
        // Change the default log level for this method. stream and area are taken
        // from the next scope up
        SENF_LOG_DEFAULTS((senf::log::DEBUG));

        SENF_LOG(("Log to Debug stream in Froblizer area however at DEBUG level"));
    }
    \endcode

    Currently, the library is not implemented in any way. The interface has been defined up to a
    point and we have dummy implementations of the 'in-code' part of the interface. This is the
    part, which is called throughout the code. The configuration API is defined but we don't even
    have a template implementation. However, this allows starting to use the SENF Logger in newly
    developed code. Even though this code will unconditionally log everything to \c std::cerr for
    now and errors in the parameter specification will not be caught (since they are just ignored)
    the logging should work automatically as advertised as soon as the logger is completely
    implemented.

    \implementation I would have much preferred a more C++ like implementation. However given the
    design goals
    \li Flexible configuration at compile and runtime
    \li Concise usage and simple interface
    \li Zero overhead for compile-time disabled log messages

    I did not find any implementation which was not either completely convoluted, unusable or
    slow. So I turned to a macro based implementation which can provide all the design goals stated
    above.
 */

#ifndef HH_Logger_
#define HH_Logger_ 1

// Custom includes
#include <iostream>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/seq/elem.hpp>

//#include "Logger.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#   ifndef _senf_LOG_STREAM
#     define _senf_LOG_STREAM std::cerr
#   endif

    /// \addtogroup logger
    /// @{

    /** \brief Write log message

        This macro will write it's last argument to the log stream. The last argument must be an
        expression which will be placed after a streaming \c operator<< (like
        <i>some-log-sttream</i> \c << <i>last-macro-arg</i>).
        \code
        SENF_LOG((parameters...)("log message " << args << ...));
        \endcode

        \hideinitializer
     */
#   define SENF_LOG(args)                                                                       \
        _senf_LOG_STREAM << BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args)       \
                         << std::endl;

    /** \brief Enable block based on logging parameters

        This macro is like SENF_LOG, however instead of writing a simple message, this macro allows
        to specify a complete block of code to be executed if the log message is enabled.
        \code
        SENF_LOG_BLOCK((parameters...)({
           // arbitrary code using 'log' for logging
           log << "log message";
        }));
        \endcode

        \hideinitializer
     */
#   define SENF_LOG_BLOCK(args)                                                 \
        do {                                                                    \
            std::ostream & log (_senf_LOG_STREAM);                              \
            BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args)       \
            log << std::endl;                                                   \
        } while (0)

    /** \brief Set scope default log parameters

        Sets the default log parameters for the current scope
        \code
        SENF_LOG_DEFAULTS((parameters...));
        \endcode

        \hideinitializer
     */
#   define SENF_LOG_DEFAULTS(args)

    /** \brief Define log area

        Defines a new log area named \a area. The area is defined as a symbol in the current scope.

        \hideinitializer
     */
#   define SENF_LOG_DEF_AREA(area)

    /** \brief Define log stream

        Defines a new log stream named \a stream. The stream is defined as a symbol in the current
        scope.

        \hideinitializer
     */
#   define SENF_LOG_DEF_STREAM(stream)

    /** \brief Define log parameter alias

        Defines a new parameter alias named \a alias as an alias for the parameters in \a args. The
        alias is defined as a symbol in the current scope.

        \hideinitializer
     */
#   define SENF_LOG_DEF_ALIAS(alias,args)

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Logger.cci"
//#include "Logger.ct"
//#include "Logger.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
