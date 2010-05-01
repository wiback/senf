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
    \brief ProgramOptions public header */

#ifndef HH_SENF_Scheduler_Console_ProgramOptions_
#define HH_SENF_Scheduler_Console_ProgramOptions_ 1

// Custom includes
#include "Config.hh"


//#include "ProgramOptions.mpp"
#include "ProgramOptions.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Console node tree based command line option parser

        A ProgramOptions instance allows flexible parsing of command line options against the
        console node tree. If you just want to parse all options, the senf::console::parseOptions()
        function will do that. ProgramOptions however allows to incrementally parse only a
        subset of the given command line options.
        \code
        std::vector<std::string> args;
        senf::console::ProgramOptions cf (argc, argv);
        cf
            .nonOptions(args)
            .alias('n', "--foo-bar=x")
            .alias('x', "--xxx", true);

        // Parse only options under the directory of some object. The object 'ob'
        // must have been registered somewhere in the node tree
        cf.parse(ob.dir);

        // Parse rest of the config file
        cf.parse();
        \endcode

        If your application uses multiple configuration sources, use a ConfigBundle and
        OptionsConfig.

        \ingroup console_access
      */
    class ProgramOptions
        : public detail::BundleMixin
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ProgramOptions(int argc, char const ** argv, DirectoryNode & root = root());
                                        ///< Create ProgramOptions parser for given options
                                        /**< The given argc/argv values are those passed to main by
                                             the operating system. Especially argv[0] is \e not an
                                             option and is ignored. */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class Container>
        ProgramOptions & nonOptions(Container & container);
                                        ///< Set container to add non-option arguments to
                                        /**< \a container must have a \c clear() and a \c
                                             push_back(std::string) member. All non-options are
                                             added to \a container. Before parsing the command-line,
                                             \a clear() is called. */
        ProgramOptions & alias(char letter, std::string const & longOpt, bool withArg=false);
                                        ///< Set short option alias
                                        /**< A short option is always an alias for a long option
                                             with or without argument. if \a withArg is \c true, the
                                             short option will expect an argument on the command
                                             line. This argument will be appended (with an
                                             additional '=') to \a longOpt. If \a withArg is \c
                                             false (the default), \a longOpt may optional contain an
                                             argument.
                                             \param[in] letter option letter
                                             \param[in] longOpt long option alias
                                             \param[in] withArg \c true, if the option should take
                                                 an argument. */

    private:
        detail::ProgramOptionsSource & config_;
    };

    /** \brief Parse command line options

        The command line otpions in \a argc / \a argv will be parsed, interpreting all node's
        relative to \a root as root node.

        \related ProgramOptions
     */
    void parseOptions(int argc, char const ** argv, DirectoryNode & root = root());

    /** \brief ConfigBundle source reading command line options

        This cosntructor is used to create aconfig source parsing the given command line options to
        add to a ConfigBundle.

        \related ProgramOptions
     */
    detail::ProgramOptionsSource::ptr OptionsConfig(int argc, char const ** argv);
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "ProgramOptions.cci"
//#include "ProgramOptions.ct"
#include "ProgramOptions.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
