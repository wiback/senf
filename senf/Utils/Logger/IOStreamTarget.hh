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
