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
    \brief Config public header */

#ifndef HH_SENF_Utils_Logger_Config_
#define HH_SENF_Utils_Logger_Config_ 1

// Custom includes
#include "Levels.hh"

//#include "Config.mpp"
#include "Config.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

/** \defgroup config Configuration

    The logger infrastructure provides for very fine-grained configuration of log messages. There
    are two parts to this configuration: compile-time configuration and runtime configuration.

    <em>Compile-time</em> configuration selects, which log statements will even be compiled. If
    logging for a certain combination of stream, area and level is disabled at compile time, no code
    will be generated for any such disabled log statement. This type of configuration is done using
    \ref SENF_LOG_CONF.

    <em>Runtime</em> configuration on the other hand deals with routing all those messages, which
    are enabled at compile time to the logging targets. If a message is not routed, it will be
    discarded. This allows to additionally disable messages at run-time. Message routing is managed
    via the ::Target interface.

    \section config_compile Compile time configuration

    Compile time configuration is set on the compiler command line:
    <pre>
    g++ ... -DSENF_LOG_CONF="(( (senf)(log)(Debug),(_),DISABLED ))
                             (( (senf)(log)(Debug),(foo)(SomeClass),VERBOSE ))
                             (( (_),(_),NOTICE ))" ...
    </pre>
    The value is relatively complex; It's a Boost.Preprocessor style sequence of tuples, of which
    the first and second elements are again sequences. What this boils down to, is that it allows to
    configure compile time logging limits based on stream and optional area.

    The above example disables all debug logging by setting the default log limit for all areas on
    the \c senf::log::Debug stream to \c DISABLED. It enables debug logging only within the \c
    foo::SomeClass area, where it is set to \c VERBOSE. Lastly, the global compile time limit is set
    to \c NOTICE.

    There are two standard uses for this configuration: Either to disable most logging in final
    builds by changing the compile time limit to something like senf::log::IMPORTANT or to enable
    senf::log::VERBOSE messages for some area:
    <pre>
    # Disable debug logging below 'IMPORTANT' level
    g++ ... -DSENF_LOG_CONF="(( (senf)(log)(Debug), (_), IMPORTANT ))"

    # Or enable verbose messages for the 'some::Area' area
    g++ ... -DSENF_LOG_CONF="(( (senf)(log)(Verbose), (some)(Area), VERBOSE ))"
    </pre>

    All the entries specified via \c SENF_LOG_CONF are applied in a fixed order:

    \li First the entries which have both a stream and an area specified are checked
    \li next all entries with area but no stream given are checked
    \li followed by all entries with a given stream but no area
    \li and lastly if no match was found until now, a generic entry without stream and area is
        checked
    \li if no matching entry is found, the default compile time limit of the stream is used

    So an area specification has precedence over a stream specification.

    \warning Enabling a message at compile time does \e not ensure, the message is shown. You
        additionally need to \e route the message (see next chapter). This is especially true for \c
        VERBOSE messages, which are default disabled at runtime.

    \see \ref SENF_LOG_CONF

    \section config_runtime Runtime configuration

    The runtime configuration is performed by routing messages to one or more logging targets:
    \code
    senf::log::ConsoleTarget & consoleLog (senf::log::ConsoleTarget::instance());
    senf::log::FileTarget fileLog ("my.log");

    consoleLog.route<senf::log::Debug>();
    consoleLog.route<foo::Transactions, foo::SomeClass>(senf::log::Target::REJECT);
    consoleLog.route<foo::Transactions, senf::log::IMPORTANT>();

    fileLog.route<foo::Transactions>();
    \endcode
    Here we see an already relatively complex setup: All debug messages (that is, those, which are
    not disabled at compile time) are routed to the console. We also route important transactions to
    the console \e except transactions from the \c foo::SomeClass area. The \c fileLog simply
    receives all transaction log messages.

    The routing statements are processed by the targets in order, the first matching rule will
    decide a log messages fate for that target.

    \warning You can \e only route those messages at runtime which have been compile-time
        enabled. By default, \c VERBOSE messages are \e disabled at compile time. They must be
        enabled \e explicitly by setting \c SENF_LOG_CONF so they can be routed.

    \section config_fallback Fallback routing

    There are two cases, where this setup may lead to inadvertently lost log messages:
    \li When using a library which does internally use the Logger but not initializing the logger in
        your application.
    \li When log messages are created during initialization of static objects.
    Since no route is set up in these cases, the messages will be dropped.

    To counter this problem, the logger is initially in <em>fallback routing</em> state. If any log
    message arrives in this state, the message will be logged to the console if it is above the
    default runtime limit of it's stream. The first routing statement on any target will take the
    logger out of this state and normal routing will take place.

    \see \ref senf::log::Target

    \section config_timesource Log message timing

    One auxiliary aspect of logging is message timing. Each message is stamped with a time-stamp
    giving the exact time the message was created. How the current date/time value is created may be
    changed by setting a \e TimeSource. A TimeSource is an instance derived from
    senf::log::TimeSource which will return the current universal time (UTC) when called.

    By default, the logging library will call gettimeofday() for each log message. To change the
    time source, just pass the new class or instance to senf::log::timeSource:
    \code
    // Use senf::scheduler::eventTime() to time log messages
    senf::log::timeSource<senf::scheduler::LogTimeSource>();
    \endcode
 */

namespace senf {
namespace log {

    ///\ingroup config
    //\{

#   ifdef DOXYGEN

    /** \brief Compile time configuration

        This define symbol sets the compile time logger configuration. This symbol should normally
        be set on the compiler command line.

        The formal syntax of this option is:

        \par ""
            <table class="ebnf">
            <tr><td>conf</td>            <td>::= \e element \e element* \n</td></tr>
            <tr><td>element</td>         <td>::= <tt>((</tt> \e optional_stream <tt>,</tt> \e optional_area <tt>,</tt> \e level <tt>))</tt> \n</td></tr>
            <tr><td>optional_stream</td> <td>::= <tt>(_)</tt> | \e scope_seq \n</td></tr>
            <tr><td>optional_area</td>   <td>::= <tt>(_)</tt> | \e scope_seq \n</td></tr>
            <tr><td>level</td>           <td>::= \c VERBOSE | \c NOTICE | \c MESSAGE | \c IMPORTANT | \c CRITICAL | \c DISABLED \n</td></tr>
            <tr><td>scope_seq</td>       <td>::= \e scope \e scope* \n</td></tr>
            <tr><td>scope</td>           <td>::= <tt>(</tt> \e name <tt>)</tt> \n</td></tr>
            <tr><td>name</td>            <td>::= arbitrary C++ identifier</td></tr>
            </table>

        \ref SENF_LOG_CONF is a Boost.Preprocessor style sequence of 3-tuples.

        The first tuple element \e optional_stream specifies the stream to match. If this is
        <tt>(_)</tt>, the entry will match any stream.

        The next tuple element, \e optional_area optionally restricts the entry to match only the
        given area. If set to <tt>(_)</tt>, the area is left unrestricted.

        The last tuple element \e level defines the compile time log level. Messages with a level
        below this are discarded at compile time.

        Both \e optional_stream and \e optional_area are given as a \e scope_seq. A scope sequence
        is a fully qualified C++ identifier placed into a sequence: <tt>foo::bar::baz</tt> is
        represented by <tt>(foo)(bar)(baz)</tt>.
     */
#   define SENF_LOG_CONF

#   endif

    /** \brief Check, if logging is enabled for stream/area/level tuple

        This is a template meta-function which will check, whether logging to the given combination
        of parameters \a Stream, \a Area and \a Level is compile-time enabled. The logging might
        still be disabled at runtime.
        \code
        if (senf::log::Enabled<senf::log::Debug,
                               senf::log::DefaultArea,
                               senf::log::VERBOSE>::value) {
            // ...
        }
        \endcode

        Since the \e value member is a compile time constant, the compiler will completely optimize
        away this block of code when logging is disabled.
     */
    template <class Stream, class Area, class Level>
    struct Enabled
    {
        static const bool value = (
            (Level::value == NONE::value ? Stream::defaultLevel::value : Level::value)
                >= detail::Config<Stream,Area>::compileLimit::value );
    };

    //\}

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Config.cci"
//#include "Config.ct"
//#include "Config.cti"
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
