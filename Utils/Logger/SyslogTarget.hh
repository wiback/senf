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

#ifndef HH_SyslogTarget_
#define HH_SyslogTarget_ 1

// Custom includes
#include <syslog.h>
#include "Target.hh"

//#include "SyslogTarget.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {
    
    class SyslogTarget 
        : public Target
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types


        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit SyslogTarget(int facility = LOG_USER);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    protected:
        void v_write(time_type timestamp, std::string const & stream, 
                     std::string const & area, unsigned level, 
                     std::string const & message);

    private:
        int facility_;
        static int const LEVELMAP_[8];
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
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
