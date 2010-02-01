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
#include <senf/Utils/Console/Console.hh>
#include <senf/Utils/Exception.hh>
#include <boost/filesystem/path.hpp>

//#include "FileTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    std::string getNodename(std::string const & filename, std::string const & nodename)
    {
        if (! nodename.empty())
            return nodename;
        return boost::filesystem::path(filename).leaf();
    }

}

prefix_ senf::log::FileTarget::FileTarget(std::string const & filename,
                                          std::string const & nodename)
    : ofstream_t (filename.c_str(), std::ofstream::app), 
      IOStreamTarget (getNodename(filename, nodename), ofstream_t::member), 
      file_ (filename)
{
    namespace fty = senf::console::factory;

    if (! ofstream_t::member)
        SENF_THROW_SYSTEM_EXCEPTION("logfile open") << ": " << filename;
    consoleDir()
        .add( "reopen",
              fty::BoundCommand(this, SENF_MEMFNP(void, FileTarget, reopen, ()))
              .doc("Reopen logfile") );
    consoleDir()
        .add("reopen", 
             fty::BoundCommand(this, SENF_MEMFNP(void, FileTarget, reopen, (std::string const &)))
             .arg("filename","new filename")
             .overloadDoc("Reopen logfile under new name") );
    consoleDir()
        .add("file", fty::Variable(boost::cref(file_))
             .doc("Show filename log messages are sent to") );
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

prefix_ std::string const & senf::log::FileTarget::filename()
    const
{
    return file_;
}

prefix_ senf::log::FileTarget::RegisterConsole::RegisterConsole()
{
    namespace kw = senf::console::kw;
    namespace fty = senf::console::factory;

    detail::TargetRegistry::instance().consoleDir()
        .add("file-target", fty::Command(&RegisterConsole::create)
             .arg("filename", "name of logfile")
             .arg("nodename", "name of node in console. Defaults to the files basename",
                  kw::default_value = "")
             .doc("Create new file target. Examples:\n"
                  "\n"
                  "Create new file target '/var/log/example.log\n"
                  "    $ file-target \"/var/log/example.log\"\n"
                  "    <Directory '/sys/log/example.log'>\n"
                  "\n"
                  "In a configuration file, create new file target '/var/log/example.log' and set\n"
                  "some parameters (If written on one line, this works at the console too:\n"
                  "    /sys/log/file-target \"/var/log/example.log\" mainlog {\n"
                  "        route (IMPORTANT);             # route all important messages\n"
                  "        timeFormat \"\";               # use non-formatted time format\n"
                  "        showArea false;                # don't show log area\n"
                  "    }\n") );
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::FileTarget::RegisterConsole::create(std::string const & filename, 
                                               std::string const & nodename)
{
    std::auto_ptr<Target> tp (new FileTarget(filename, nodename));
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
