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
    \brief ParsedCommand non-inline non-template implementation */

#include "ParsedCommand.hh"
#include "ParsedCommand.ih"

// Custom includes

#include "ParsedCommand.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::ParsedCommandOverloadBase

prefix_ unsigned senf::console::ParsedCommandOverloadBase::v_numArguments()
    const
{
    return parameters_.size();
}

prefix_ void senf::console::ParsedCommandOverloadBase::v_argumentDoc(unsigned index,
                                                                     ArgumentDoc & doc)
    const
{
    BOOST_ASSERT( index < parameters_.size() );
    detail::ArgumentInfoBase & arg (*parameters_[index]);
    doc.name = arg.name.empty() 
        ? (boost::format("arg%d%d") % overloadIndex() % (index+1)).str()
        : arg.name;
    doc.type = arg.type;
    doc.defaultValue = arg.defaultValueStr();
    doc.doc = arg.doc;
}

prefix_ std::string senf::console::ParsedCommandOverloadBase::v_doc()
    const
{
    return doc_;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
#include "ParsedCommand.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
