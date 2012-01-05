// $Id$
//
// Copyright (C) 2009
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
    \brief DumpFormat non-inline non-template implementation */

#include "Packets.hh"
//#include "DumpFormat.ih"

// Custom includes
#include <iostream>
#include <algorithm>
#include <senf/config.hh>

//#include "DumpFormat.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::string senf::fieldName(std::string const & s)
{
    std::string t (std::max(std::string::size_type(SENF_PACKET_DUMP_COLON_COLUMN+1), s.size()+5), ' ');
    std::copy(s.begin(), s.end(), t.begin()+2);
    t[t.size()-2] = ':';
    return t;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "DumpFormat.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
