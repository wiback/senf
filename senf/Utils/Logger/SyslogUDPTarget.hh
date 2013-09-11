// $Id$
//
// Copyright (C) 2008
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
    \brief SyslogUDPTarget public header */

#ifndef HH_SENF_Utils_Logger_SyslogUDPTarget_
#define HH_SENF_Utils_Logger_SyslogUDPTarget_ 1

// Custom includes
#include <syslog.h>
#include <senf/Socket/Protocols/INet/INetAddressing.hh>
#include <senf/Socket/ClientSocketHandle.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include "LogFormat.hh"
#include "Target.hh"

//#include "SyslogUDPTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Log target writing UDP syslog packets

        The SyslogUDPTarget will send all %log messages directly via UDP to a target host. This
        host should have a syslog daemon or relay running. The protocol is defined in <a
        href="http://tools.ietf.org/html/rfc3164">RFC-3164</a>.

        This %log target has some important benefits:

        \li It will never block. It may however lose %log messages.
        \li It does \e not add timestamp information locally.

        These are \e advantages since this makes SyslogUDPTarget a very reliable high-performance
        logging target.

        Valid facility values are from <tt>man 3 syslog</tt>:

        \par ""
           <tt>LOG_AUTHPRIV</tt>, <tt>LOG_CRON</tt>, <tt>LOG_DAEMON</tt>, <tt>LOG_FTP</tt>,
           <tt>LOG_KERN</tt>, <tt>LOG_LOCAL0</tt>, <tt>LOG_LOCAL1</tt>, <tt>LOG_LOCAL2</tt>,
           <tt>LOG_LOCAL3</tt>, <tt>LOG_LOCAL4</tt>, <tt>LOG_LOCAL5</tt>, <tt>LOG_LOCAL6</tt>,
           <tt>LOG_LOCAL7</tt>, <tt>LOG_LPR</tt>, <tt>LOG_MAIL</tt>, <tt>LOG_NEWS</tt>,
           <tt>LOG_SYSLOG</tt>, <tt>LOG_USER</tt>, <tt>LOG_UUCP</tt>

        the default facility is <tt>LOG_USER</tt>

        The SENF %log levels are mapped to syslog levels in the following way:

        <table class="senf fixedcolumn">
        <tr><td>senf::log::VERBOSE</td>   <td>\c LOG_DEBUG</td></tr>
        <tr><td>senf::log::NOTICE</td>    <td>\c LOG_INFO</td></tr>
        <tr><td>senf::log::MESSAGE</td>   <td>\c LOG_NOTICE</td></tr>
        <tr><td>senf::log::IMPORTANT</td> <td>\c LOG_WARNING</td></tr>
        <tr><td>senf::log::CRITICAL</td>  <td>\c LOG_CRIT</td></tr>
        <tr><td>senf::log::FATAL</td>     <td>\c LOG_EMERG</td></tr>
        </table>

        \note Since the UDP syslog packets are limited to 1024 characters and there must be some
            space left so a relay may optionally add a timestamp and hostname section, the %log
            messages are split after 896 characters. Additionally the %log messages are split at each
            newline char since non-printable characters are not allowed.

        \implementation The RFC only \e recommends the exact message format. This allows us to
            include the \c PRI part but skip the \c HEADER part (which includes the timestamp and
            hostname) for better performance. We add a space after the \c PRI to force the syslog
            daemon to skip the \c HEADER part.

        \ingroup targets
     */
    class SyslogUDPTarget
        : public Target, private detail::LogFormat
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit SyslogUDPTarget(INet4Address const & target, int facility = LOG_USER);
        explicit SyslogUDPTarget(INet4SocketAddress const & target, int facility = LOG_USER);
        explicit SyslogUDPTarget(INet6Address const & target, int facility = LOG_USER);
        explicit SyslogUDPTarget(INet6SocketAddress const & target, int facility = LOG_USER);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        using detail::LogFormat::showTime;
        using detail::LogFormat::showStream;
        using detail::LogFormat::showLevel;
        using detail::LogFormat::showArea;
        using detail::LogFormat::timeFormat;
        using detail::LogFormat::tag;

        bool syslog() const;            ///< \c true, if using syslog format, \c false otherwise
                                        /**< When syslog format is disabled, messages are not
                                             formated as valid syslog messages but sent using plain
                                             UDP. */
        void syslog(bool enabled=true); ///< Set syslog format

    private:
        void init();
        void v_write(time_type timestamp, std::string const & stream,
                     std::string const & area, unsigned level,
                     std::string const & message);

        void consoleFormat(std::ostream & os);

        int facility_;
        typedef senf::ClientSocketHandle< senf::MakeSocketPolicy<
            senf::DatagramFramingPolicy,
            senf::ConnectedCommunicationPolicy,
            senf::WriteablePolicy>::policy > Handle;
        Handle handle_;
        bool syslogFormat_;

    public:
        enum LogFacility {
            AUTHPRIV = LOG_AUTHPRIV,
            CRON = LOG_CRON,
            DAEMON = LOG_DAEMON,
            FTP = LOG_FTP,
            KERN = LOG_KERN,
            LOCAL0 = LOG_LOCAL0,
            LOCAL1 = LOG_LOCAL1,
            LOCAL2 = LOG_LOCAL2,
            LOCAL3 = LOG_LOCAL3,
            LOCAL4 = LOG_LOCAL4,
            LOCAL5 = LOG_LOCAL5,
            LOCAL6 = LOG_LOCAL6,
            LOCAL7 = LOG_LOCAL7,
            LPR = LOG_LPR,
            MAIL = LOG_MAIL,
            NEWS = LOG_NEWS,
            SYSLOG = LOG_SYSLOG,
            USER = LOG_USER,
            UUCP = LOG_UUCP
        };

    private:

        struct RegisterConsole {
            RegisterConsole();
            static boost::shared_ptr<console::DirectoryNode> create(
                INet4SocketAddress const & target, LogFacility facility = USER);
            static boost::shared_ptr<console::DirectoryNode> create(
                INet4Address const & target, LogFacility facility = USER);
            static boost::shared_ptr<console::DirectoryNode> create(
                INet6SocketAddress const & target, LogFacility facility = USER);
            static boost::shared_ptr<console::DirectoryNode> create(
                INet6Address const & target, LogFacility facility = USER);
            static RegisterConsole instance;
        };
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "SyslogUDPTarget.cci"
//#include "SyslogUDPTarget.ct"
//#include "SyslogUDPTarget.cti"
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
