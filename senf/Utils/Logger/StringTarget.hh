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
    \brief StringTarget public header */

#ifndef HH_SENF_Utils_Logger_StringTarget_
#define HH_SENF_Utils_Logger_StringTarget_ 1

// Custom includes
#include <sstream>
#include "IOStreamTarget.hh"

//#include "StringTarget.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Store %log messages in a string buffer

        This target is mostly useful for debug purposes.

        \ingroup targets
      */
    class StringTarget
        : private boost::base_from_member<std::stringstream>,
          public IOStreamTarget
    {
        typedef boost::base_from_member<std::stringstream> stream_base;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        StringTarget();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        std::string str() const;        ///< Get log messages accumulated so far
        void clear();                   ///< Clear buffer

    protected:

    private:

    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "StringTarget.cci"
//#include "StringTarget.ct"
#include "StringTarget.cti"
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
