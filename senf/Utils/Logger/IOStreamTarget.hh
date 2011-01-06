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
#include "Target.hh"
#include "LogFormat.hh"

//#include "IOStreamTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        : public Target, private detail::LogFormat
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        IOStreamTarget(std::string const & name, std::ostream & os);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        using detail::LogFormat::showTime;
        using detail::LogFormat::showStream;
        using detail::LogFormat::showLevel;
        using detail::LogFormat::showArea;
        using detail::LogFormat::timeFormat;
        using detail::LogFormat::tag;

    protected:
        void v_write(time_type timestamp, std::string const & stream,
                     std::string const & area, unsigned level,
                     std::string const & message);

    private:
        std::ostream & stream_;

    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
