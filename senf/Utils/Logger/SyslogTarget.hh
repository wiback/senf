// $Id$
//
// Copyright (C) 2008
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
    \brief SyslogTarget public header */

#ifndef HH_SENF_Utils_Logger_SyslogTarget_
#define HH_SENF_Utils_Logger_SyslogTarget_ 1

// Custom includes
#include <syslog.h>
#include <boost/shared_ptr.hpp>
#include "Target.hh"

//#include "SyslogTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    namespace console { class DirectoryNode; }

namespace log {

    /** \brief Log target writing to the syslog

        The SyslogTarget will send all %log messages to the syslog at the given facility.

        \code
        senf::log::SyslogTarget syslog;

        // Route all messages to the syslog
        syslog.route();
        \endcode

        Valid facility values (taken from <tt>man 3 syslog</tt>):
        \par ""
           <tt>LOG_AUTHPRIV</tt>, <tt>LOG_CRON</tt>, <tt>LOG_DAEMON</tt>, <tt>LOG_FTP</tt>,
           <tt>LOG_KERN</tt>, <tt>LOG_LOCAL0</tt>, <tt>LOG_LOCAL1</tt>, <tt>LOG_LOCAL2</tt>,
           <tt>LOG_LOCAL3</tt>, <tt>LOG_LOCAL4</tt>, <tt>LOG_LOCAL5</tt>, <tt>LOG_LOCAL6</tt>,
           <tt>LOG_LOCAL7</tt>, <tt>LOG_LPR</tt>, <tt>LOG_MAIL</tt>, <tt>LOG_NEWS</tt>,
           <tt>LOG_SYSLOG</tt>, <tt>LOG_USER</tt>, <tt>LOG_UUCP</tt>

        The default facility is <tt>LOG_USER</tt>.

        The SENF log levels are mapped to syslog levels in the following way:

        <table class="senf fixedcolumn">
        <tr><td>senf::log::VERBOSE</td>   <td>\c LOG_DEBUG</td></tr>
        <tr><td>senf::log::NOTICE</td>    <td>\c LOG_INFO</td></tr>
        <tr><td>senf::log::MESSAGE</td>   <td>\c LOG_NOTICE</td></tr>
        <tr><td>senf::log::IMPORTANT</td> <td>\c LOG_WARNING</td></tr>
        <tr><td>senf::log::CRITICAL</td>  <td>\c LOG_CRIT</td></tr>
        <tr><td>senf::log::FATAL</td>     <td>\c LOG_EMERG</td></tr>
        </table>

        \ingroup targets
     */
    class SyslogTarget
        : public Target
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit SyslogTarget(int facility = LOG_USER);

        //\}
        //-////////////////////////////////////////////////////////////////////////

    private:
        void v_write(time_type timestamp, std::string const & stream,
                     std::string const & area, unsigned level,
                     std::string const & message);

        int facility_;

    public:
        static int const LEVELMAP[8];

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
                LogFacility facility);
            static RegisterConsole instance;
        };
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "SyslogTarget.cci"
//#include "SyslogTarget.ct"
//#include "SyslogTarget.cti"
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
