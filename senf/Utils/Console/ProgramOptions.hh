// $Id$
//
// Copyright (C) 2008
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
    \brief ProgramOptions public header */

#ifndef HH_SENF_Utils_Console_ProgramOptions_
#define HH_SENF_Utils_Console_ProgramOptions_ 1

// Custom includes
#include "Config.hh"


//#include "ProgramOptions.mpp"
#include "ProgramOptions.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        ProgramOptions(int argc, char const ** argv, DirectoryNode & root = senf::console::root());
                                        ///< Create ProgramOptions parser for given options
                                        /**< The given argc/argv values are those passed to main by
                                             the operating system. Especially argv[0] is \e not an
                                             option and is ignored. */

        //\}
        //-////////////////////////////////////////////////////////////////////////

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

        The command line options in \a argc / \a argv will be parsed, interpreting all node's
        relative to \a root as root node.

        \related ProgramOptions
     */
    void parseOptions(int argc, char const ** argv, DirectoryNode & root = senf::console::root());

    /** \brief ConfigBundle source reading command line options

        This constructor is used to create a config source parsing the given command line options to
        add to a ConfigBundle.

        \related ProgramOptions
     */
    detail::ProgramOptionsSource::ptr OptionsConfig(int argc, char const ** argv);
}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
