// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief SysInfo public header */

#ifndef HH_SENF_Scheduler_Console_SysInfo_
#define HH_SENF_Scheduler_Console_SysInfo_ 1

// Custom includes
#include <list>
#include <senf/Utils/singleton.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {
namespace console {

    class SysInfo
        : public singleton<SysInfo>
    {
    public:
        friend class singleton<SysInfo>;
        using singleton<SysInfo>::instance;

        void addEntry(std::string const & descr, unsigned pos=-1);
        void dump(std::ostream & os) const;

        struct Proxy {
            Proxy(std::string const & descr, unsigned pos=-1);
        };

    private:
        std::list<std::string> descr_;

        SysInfo();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "SysInfo.cci"
//#include "SysInfo.ct"
//#include "SysInfo.cti"
#endif
