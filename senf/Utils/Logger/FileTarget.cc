// $Id$
//
// Copyright (C) 2007
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
    \brief FileTarget non-inline non-template implementation */

#include "FileTarget.hh"
//#include "FileTarget.ih"

// Custom includes
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Variables.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>

//#include "FileTarget.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    std::string getNodename(std::string const & filename, std::string const & nodename)
    {
        if (! nodename.empty())
            return nodename;
#if BOOST_VERSION >= 104600
        return boost::filesystem::path(filename).leaf().string();
#else
        return boost::filesystem::path(filename).leaf();
#endif
    }

}

prefix_ senf::log::FileTarget::FileTarget(std::string const & filename,
                                          std::string const & nodename)
    : ofstream_t (filename.c_str(), std::ofstream::app),
      IOStreamTarget (getNodename(filename, nodename), ofstream_t::member),
      file_ (filename)
{
    namespace fty = console::factory;

    if (! ofstream_t::member)
        SENF_THROW_SYSTEM_EXCEPTION("logfile open") << ": " << filename;
    consoleDir()
        .add( "reopen",
              fty::Command(SENF_MEMBINDFNP(void, FileTarget, reopen, ()))
              .doc("Reopen logfile") );
    consoleDir()
        .add("reopen",
             fty::Command(SENF_MEMBINDFNP(void, FileTarget, reopen, (std::string const &)))
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
    namespace kw = console::kw;
    namespace fty = console::factory;

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
    unique_or_auto_ptr<Target> tp (new FileTarget(filename, nodename));
    Target & target (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(move_or_noop(tp));
    return target.consoleDir().node().thisptr();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
