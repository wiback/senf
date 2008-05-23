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
    \brief ProgramOptions non-inline non-template implementation */

#include "ProgramOptions.hh"
#include "ProgramOptions.ih"

// Custom includes
#include <boost/algorithm/string/predicate.hpp>

//#include "ProgramOptions.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::ProgramOptionsSource

prefix_ void senf::console::detail::ProgramOptionsSource::v_parse(RestrictedExecutor & executor)
{
    char ** argp (argv_);
    int n (argc_);
    for (; n; --n, ++argp) {
        std::string arg (*argp);
        if (boost::algorithm::starts_with(arg, std::string("--")))
            parseLongOption(arg.substr(2), executor);
        else
            parseNonOption(arg, executor);
    }
}

namespace {
    struct MakeWordToken
    {
        typedef senf::console::Token result_type;
        template <class Range>
        result_type operator()(Range const & r) const
            { return senf::console::WordToken(std::string(boost::begin(r), boost::end(r))); }
    };
}

prefix_ void
senf::console::detail::ProgramOptionsSource::parseLongOption(std::string const & arg,
                                                             RestrictedExecutor & executor)
{
    std::string::size_type ix (arg.find('='));
    std::string name (arg.substr(0,ix));
    std::string value (ix==std::string::npos ? std::string() : arg.substr(ix+1));

    typedef std::vector<Token> Path;

    ParseCommandInfo cmd;
    Path path;
    
    DirectoryNode::ptr cwd (executor.root().thisptr());
    std::string::size_type b (0);
    while (b < name.size()) {
        std::string::size_type e (name.size());
        for (;e != std::string::npos && e > b; e = name.rfind('-', e)) {
            DirectoryNode::ChildrenRange completions (cwd->completions(name.substr(b,e-b)));
            if (completions.size() == 1) {
                path.push_back(WordToken(completions.begin()->first));
                if (e < name.size())
                    cwd = cwd->getDirectory(completions.begin()->first).thisptr();
                b = e+1;
                e = b+1;
                break;
            }
        }
        if (e == std::string::npos || e <= b) {
            // This will produce a correct error message later
            path.push_back(WordToken(name.substr(b)));
            b = name.size();
        }
    }
    
    cmd.command(path);
    parser_.parseArguments(value, cmd);
    executor(std::cerr, cmd);
}

prefix_ void
senf::console::detail::ProgramOptionsSource::parseNonOption(std::string const & arg,
                                                            RestrictedExecutor & executor)
{}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::console::parseOptions(int argc, char ** argv, DirectoryNode & root)
{
    ProgramOptions opts (argc, argv, root);
    opts.parse();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ProgramOptions.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
