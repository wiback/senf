// $Id$
//
// Copyright (C) 2007
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
    \brief IOStreamTarget public header */

#ifndef HH_SENF_Utils_Logger_IOStreamTarget_
#define HH_SENF_Utils_Logger_IOStreamTarget_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include "Target.hh"

//#include "IOStreamTarget.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Write %log messages to arbitrary std::ostream

        This target will write %log messages to an arbitrary std::ostream in the format
        \verbatim <date> [<level>][<area>] <message> \endverbatim

        The \e area will be omitted if it is \c senf::log::DefaultArea.
        
        \warning The class keeps a reference to the passed stream.
        
        \ingroup targets
      */
    class IOStreamTarget
        : public Target
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit IOStreamTarget(std::ostream & os);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

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
        
    protected:
        void v_write(time_type timestamp, std::string const & stream, 
                     std::string const & area, unsigned level, 
                     std::string const & message);

    private:
        std::ostream & stream_;

        std::stringstream datestream_;
        bool noformat_;
        bool showTime_;
        bool showStream_;
        bool showLevel_;
        bool showArea_;

    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "IOStreamTarget.cci"
//#include "IOStreamTarget.ct"
//#include "IOStreamTarget.cti"
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
