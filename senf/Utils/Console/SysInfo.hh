// $Id$
//
// Copyright (C) 2010
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief SysInfo public header */

#ifndef HH_SENF_Scheduler_Console_SysInfo_
#define HH_SENF_Scheduler_Console_SysInfo_ 1

// Custom includes
#include <map>
#include <string>
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

        void addEntry(std::string const & descr, int pos=-1);
        void dump(std::ostream & os) const;

        struct Proxy {
            Proxy(std::string const & descr, int pos=-1);
        };

    private:
        typedef std::multimap<int, std::string> Descriptions;
        Descriptions descr_;

        SysInfo();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "SysInfo.cci"
//#include "SysInfo.ct"
//#include "SysInfo.cti"
#endif
