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
    \brief Log public header */

#ifndef HH_SENF_Utils_Logger_Log_
#define HH_SENF_Utils_Logger_Log_ 1

// Custom includes
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/pop_back.hpp>
#include "Parameters.hh"

//#include "Log.mpp"
#include "Log.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \defgroup logging Logging commands

    The logging library provides several commands to create %log messages. All these macro commands
    take a variable number of arguments. Since this is not supported in a usable way by the C++
    preprocessor, the arguments are encoded into a <a
    href="http://www.boost.org/doc/libs/release/libs/preprocessor/doc/index.html">Boost.Preprocessor</a>
    like sequence:

    \code
    SENF_LOG( (senf::log::Debug)(senf::log::NOTICE)(FroblizerArea)("The log message") );
    \endcode

    The argument is comprised of a sequence of parameters and the %log message itself.
    The parameters are
    - the <em>%log stream</em>,
    - the <em>%log area</em>,
    - the <em>%log level</em>.

    These parameters are optional and may be specified <i>in arbitrary order</i> (with the %log
    message always being the last sequence element) and even multiple times in the parameter
    sequence. If some argument type occurs multiple times, the last occurrence wins. If any one of
    the parameters is not specified, it's current default value will be used.

    This current default value is set using \ref SENF_LOG_DEFAULT_STREAM, \ref SENF_LOG_DEFAULT_AREA
    and \ref SENF_LOG_DEFAULT_LEVEL respectively. These macros set the default stream, area and/or
    level <em>of the current scope</em>. They may be used with a class declaration to set defaults
    for all class members or within a function or member body to set the default for that member
    only. They may be used only \e once within each scope.

    The logging library defines the global defaults for stream, area and level to be \c
    senf::log::Debug, senf::log::DefaultArea, and senf::log::NONE respectively.

    The %log level senf::log::NONE is special. If the %log level is set to this value, the %log level
    will be set from the stream provided default value.

    All these parameters must be <em>compile time constants</em> (they are all types, so it's
    difficult for them to be something else).

    \section logging_aliases Aliases

    To further simplify logging commands, aliases may be defined within any scope. An alias is an
    arbitrary collection of %log parameters:
    \code
    SENF_LOG_DEFINE_ALIAS( VerboseDebug, (senf::log::Debug)(senf::log::VERBOSE) );
    \endcode
    Within %log statements, aliases may be used like normal parameters. They will be substituted for
    the parameter sequence they represent:
    \code
    SENF_LOG( (VerboseDebug)("Debug message") )
    // is equivalent to
    SENF_LOG( (senf::log::Debug)(senf::log::VERBOSE)("Debug message") )
    \endcode
    Aliases may be used together with other parameters, even with further aliases in any order.
 */

///\addtogroup logging
//\{

///\name Generating log messages
//\{

/** \brief Write log message

    This macro will write it's last argument to the log stream. The last argument must be an
    expression which will be placed after a streaming \c operator<< (like
    <i>some-log-stream</i> \c << <i>last-macro-arg</i>).
    \code
    SENF_LOG((parameters...)("log message " << args << ...));
    \endcode

    \hideinitializer
 */
#define SENF_LOG(args)                                                                            \
    SENF_LOG_BLOCK_( SENF_LOG_MERGE_PARAMETERS(BOOST_PP_SEQ_POP_BACK(args)),                      \
                     { log << BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args); })

/** \brief Write log message (template context)

    This macro is used like \ref SENF_LOG() if called from a template context

    \hideinitializer
 */
#define SENF_LOG_TPL(args)                                                                        \
    SENF_LOG_BLOCK_TPL_( SENF_LOG_MERGE_PARAMETERS_TPL(BOOST_PP_SEQ_POP_BACK(args)),              \
                         { log << BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args); })

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
#define SENF_LOG_BLOCK(args)                                                                      \
    SENF_LOG_BLOCK_( SENF_LOG_MERGE_PARAMETERS(BOOST_PP_SEQ_POP_BACK(args)),                      \
                     BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args))

/** \brief Write log message (template context)

    This macro is used like \ref SENF_LOG_BLOCK() if called from a template context

    \hideinitializer
 */
#define SENF_LOG_BLOCK_TPL(args)                                                                  \
    SENF_LOG_BLOCK_TPL_( SENF_LOG_MERGE_PARAMETERS_TPL(BOOST_PP_SEQ_POP_BACK(args)),              \
                         BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args))

//\}
//\}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Log.cci"
//#include "Log.ct"
//#include "Log.cti"
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
