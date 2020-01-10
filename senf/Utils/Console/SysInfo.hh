//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief SysInfo public header */

#ifndef HH_SENF_Utils_Console_SysInfo_
#define HH_SENF_Utils_Console_SysInfo_ 1

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

        void setEntry(std::string const & name, std::string const & entry, int pos = -1);
        void dump(std::ostream & os) const;

        struct Proxy {
            Proxy(std::string const & name, std::string const & entry, int pos = -1);
        };

    private:
        typedef std::multimap<std::pair<int,std::string>, std::string> Entries;
        Entries entries_;

        SysInfo();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "SysInfo.cci"
//#include "SysInfo.ct"
//#include "SysInfo.cti"
#endif
