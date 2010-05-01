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
    \brief ConsoleTarget public header */

#ifndef HH_SENF_Utils_Logger_ConsoleTarget_
#define HH_SENF_Utils_Logger_ConsoleTarget_ 1

// Custom includes
#include "IOStreamTarget.hh"

//#include "ConsoleTarget.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Write %log messages to std::cout

        IOStreamTarget writing to std::cout. This is a singleton target which always exists. Access
        it via senf::log::ConsoleTarget::instance()
        \code
        senf::log::ConsoleTarget & console (senf::log::ConsoleTarget::instance());

        console.route<senf::log::Debug>();
        \endcode

        \ingroup targets
     */
    class ConsoleTarget : public IOStreamTarget
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static ConsoleTarget & instance();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    private:
        ConsoleTarget();
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "ConsoleTarget.cci"
//#include "ConsoleTarget.ct"
//#include "ConsoleTarget.cti"
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
