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
        std::stringstream datestream_;
        std::string tag_;
        std::string timeFormat_;
        time_type timeBase_;

        bool noformat_;
        bool showTime_;
        bool showStream_;
        bool showLevel_;
        bool showArea_;
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
