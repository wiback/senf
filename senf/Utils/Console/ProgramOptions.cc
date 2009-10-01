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
#include <boost/format.hpp>
#include <senf/Utils/Range.hh>
#include "OverloadedCommand.hh"

//#include "ProgramOptions.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::ProgramOptionsSource::NonOptionContainer

prefix_ senf::console::detail::ProgramOptionsSource::NonOptionContainer::~NonOptionContainer()
{}

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::ProgramOptionsSource

prefix_ void senf::console::detail::ProgramOptionsSource::v_parse(RestrictedExecutor & executor)
{
    if (nonOptions_)
        nonOptions_->clear();
    if (argc_ <= 1)
        return;
    char const ** argp (argv_+1);
    int n (argc_-1);
    for (; n; --n, ++argp) {
        std::string arg (*argp);
        if (arg == "--") {
            for (; n; --n, ++argp)
                parseNonOption(arg, executor);
            break;
        }
        else if (boost::algorithm::starts_with(arg, std::string("--")) && arg.size() > 2)
            try {
                parseLongOption(arg.substr(2), executor);
            }
            catch (senf::ExceptionMixin & e) {
                e << "\nwhile parsing command line option: " << arg;
                throw;
            }
        else if (boost::algorithm::starts_with(arg, std::string("-")) && arg.size() > 1) {
            for (std::string::size_type i (1); i<arg.size(); ++i) {
                char opt (arg[i]);
                ShortOptions::iterator j (shortOptions_.find(opt));
                if (j == shortOptions_.end())
                    throw SyntaxErrorException(
                        (boost::format("invalid short option '%c'") % opt).str());
                std::string param;
                if (j->second.withArg) {
                    if (i >= arg.size()-1) {
                        if (n > 0) {
                            param = *(++argp);
                            --n;
                        }
                    }
                    else 
                        param = arg.substr(i+1);
                    i = arg.size();
                }
                std::string longOpt (j->second.longOpt);
                if (! param.empty() ) {
                    longOpt += "=";
                    longOpt += param;
                }
                if (boost::algorithm::starts_with(longOpt, std::string("--")))
                    longOpt = longOpt.substr(2);
                try {
                    parseLongOption(longOpt, executor);
                }
                catch (senf::ExceptionMixin & e) {
                    e << "\nwhile parsing command line option: -" << opt << ' ' << param;
                    throw;
                }
            }
        }
        else
            parseNonOption(arg, executor);
    }
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
            std::string key (name.substr(b,e-b));
            if (! cwd->hasChild(key)) {
                DirectoryNode::ChildrenRange completions (cwd->completions(key));
                if (has_one_elt(completions))
                    key = completions.begin()->first;
                else {
                    e -= 1;
                    continue;
                }
            }
            path.push_back(WordToken(key));
            if (e < name.size())
                cwd = cwd->getDirectory(key).thisptr();
            b = e+1;
            e = b+1;
            break;
        }
        if (e == std::string::npos || e <= b) {
            // This will produce a correct error message later or will skip the node,
            // if parsing is restricted to a subtree
            path.push_back(WordToken(name.substr(b)));
            b = name.size();
        }
    }

    cmd.command(path);
    // Here we check, whether the command
    // - is an overloaded/parsed command
    // - with a single overload
    // - taking only a single argument
    // - which consists of a single token
    // If all these conditions are met, we pass the parameter value as a single WordToken
    // otherwise we parse it using the config parser
    try {
        GenericNode const & node (executor.getNode(cmd));
        OverloadedCommandNode const * cmdnode (dynamic_cast<OverloadedCommandNode const *>(&node));
        if (cmdnode && cmdnode->overloads().size() == 1) {
            CommandOverload const & overload (**cmdnode->overloads().begin());
            if (overload.numArguments() == 1) {
                ArgumentDoc argdoc;
                argdoc.singleToken = false;
                overload.argumentDoc(0, argdoc);
                if (argdoc.singleToken) {
                    cmd.addToken(WordToken(value));
                    goto execute;
                }
            }
        } /* else */ {
            parser_.parseArguments(value, cmd);
        }
    execute:
        executor(executor.stream(), cmd);
    }
    catch (Executor::IgnoreCommandException &)
    {}
}

prefix_ void
senf::console::detail::ProgramOptionsSource::parseNonOption(std::string const & arg,
                                                            RestrictedExecutor & executor)
{
    if (! nonOptions_)
        throw SyntaxErrorException("invalid non-option argument");
    nonOptions_->push_back(arg);
}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::console::parseOptions(int argc, char const ** argv, DirectoryNode & root)
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
