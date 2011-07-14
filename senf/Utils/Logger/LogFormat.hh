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
    \brief LogFormat public header */

#ifndef HH_SENF_Utils_Logger_LogFormat_
#define HH_SENF_Utils_Logger_LogFormat_ 1

// Custom includes
#include <sstream>
#include "TimeSource.hh"
#include <senf/Utils/Console/LazyDirectory.hh>

//#include "LogFormat.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {
namespace detail {

    class LogFormat
    {
    public:
        LogFormat();
        explicit LogFormat(console::ScopedDirectory<> & dir);

        void showTime(bool flag = true); ///< Enable or disable output of time field
        void showStream(bool flag = true); ///< Enable or disable output of stream field
        void showLevel(bool flag = true); ///< Enable or disable output of log level
        void showArea(bool flag = true); ///< Enable or disable output of log area

        void timeFormat(std::string const & format);
                                        ///< Set time format
                                        /**< The date formatting is set using the Boost.DateTime
                                             date_facet, e.g.
                                             \code
                                                 target.timeFormat("%Y%m%d %H:%M:%S");
                                             \endcode
                                             If the \c timeFormat is set to the empty string, the
                                             time is written out as unformatted ClockService value.

                                             By default, the date-time will be written in extended
                                             ISO format.
                                             \param[in] format Date/Time format string */

        void tag(std::string const & tag); ///< Set tag (log line prefix)

    protected:
        std::string prefix(time_type timestamp, std::string const & stream,
                           std::string const & area, unsigned level);
        bool isPlainFormat() const;
        void consoleFormat(std::ostream & os);

    private:

        std::string tag_;
        std::string timeFormat_;
        std::stringstream datestream_;
        bool noformat_;
        bool showTime_;
        bool showStream_;
        bool showLevel_;
        bool showArea_;
        time_type timeBase_;
    };

    void quoteNonPrintable(std::string & s);
    std::string getDefaultTag();

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "LogFormat.cci"
//#include "LogFormat.ct"
//#include "LogFormat.cti"
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
