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
    \brief FileTarget non-inline non-template implementation */

#include "FileTarget.hh"
//#include "FileTarget.ih"

// Custom includes
#include "../Console/Console.hh"

//#include "FileTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    std::string quoteFilename(std::string filename)
    {
        for (std::string::iterator i (filename.begin()); i != filename.end(); ++i)
            if (! senf::console::CommandParser::isWordChar(*i))
                *i = '_';
        return filename;
    }

}

prefix_ senf::log::FileTarget::FileTarget(std::string const & file)
    : ofstream_t(file.c_str(), std::ofstream::app), 
      IOStreamTarget(quoteFilename(file), ofstream_t::member), 
      file_(file)
{
    consoleDir().add( "reopen", senf::membind(
                          static_cast<void (FileTarget::*)()>(&FileTarget::reopen),
                          this))
        .doc("Reopen logfile");
    consoleDir().add("reopen", senf::membind(
                         static_cast<void (FileTarget::*)(std::string const &)>(&FileTarget::reopen),
                         this))
        .arg("filename","new filename")
        .overloadDoc("Reopen logfile under new name");
}

prefix_ void senf::log::FileTarget::reopen()
{
    ofstream_t::member.close();
    ofstream_t::member.open(file_.c_str(), std::ofstream::app);
}

prefix_ void senf::log::FileTarget::reopen(std::string const & file)
{
    file_ = file;
    reopen();
    // Rename console directory
    console::DirectoryNode::ptr parent (consoleDir().node().parent());
    if (parent)
        parent->add(file, consoleDir().node().unlink());
}

prefix_ senf::log::FileTarget::RegisterConsole::RegisterConsole()
{
    detail::TargetRegistry::instance().consoleDir().add("file-target",&RegisterConsole::create)
        .arg("filename", "name of logfile")
        .doc("Create new file target.");
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::FileTarget::RegisterConsole::create(std::string const & filename)
{
    std::auto_ptr<Target> tp (new FileTarget(filename));
    Target & target (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(tp);
    return target.consoleDir().node().thisptr();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FileTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
