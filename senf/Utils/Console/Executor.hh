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
    \brief Executor public header */

#ifndef HH_SENF_Scheduler_Console_Executor_
#define HH_SENF_Scheduler_Console_Executor_ 1

// Custom includes
#include <boost/noncopyable.hpp>
#include "Parse.hh"
#include <senf/Utils/Logger/SenfLog.hh>
#include "Node.hh"

//#include "Executor.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Execute config/console commands

        The Executor interprets parsed config/console commands and executes them. It manages the
        current execution context (current directory, directory stack and so on).

        The executor is normally called directly by the parser callback for each command.

        Executing the built-in 'exit' command will throw Executor::ExitException. This exception
        (which is not derived from std::exception since it's not a real exception) must be handled
        by the caller.

        All directories are managed using weak pointers. If any of the directories expires (current
        directory, directory stack, last directory) it will be replaced with the root
        directory. Directories expire when they are destructed or when they are detached from the
        config tree root node.
      */
    class Executor
        : boost::noncopyable
    {
        SENF_LOG_CLASS_AREA();
        SENF_LOG_DEFAULT_LEVEL( senf::log::VERBOSE );
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        /// Thrown by built-in 'exit' command
        struct ExitException {};

        /// Executor policy function
        typedef boost::function<void (DirectoryNode &,std::string const &)> SecurityPolicy;

        /// Thrown by the SecurityPolicy to silently ignore a command
        struct IgnoreCommandException {};

        typedef void result_type;

        //-////////////////////////////////////////////////////////////////////////
        //\/name Structors and default members
        //\{

        Executor();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void execute(std::ostream & output, ParseCommandInfo const & command);
                                        ///< Execute command
                                        /**< Output will be written to \a output.
                                             Same as operator()(). */

        void operator()(std::ostream & output, ParseCommandInfo const & command);
                                        ///< Execute command
                                        /**< Output will be written to \a output.
                                             Same as execute(). */
        GenericNode & getNode(ParseCommandInfo const & command);
        DirectoryNode & cwd() const;    ///< Current working directory
        void cwd(DirectoryNode & dir);  ///< Change current directory
        std::string cwdPath() const;    ///< Return pathname of current directory
        bool skipping() const;          ///< \c true, if currently skipping a directory group

        bool autocd() const;            ///< Get current autocd status
                                        /**< if autocd is enabled, specifying a directory name as
                                             command will cd to that directory. Disabled by
                                             default (but enabled automatically by the interactive
                                             console). */
        Executor & autocd(bool v);      ///< Set autocd status
                                        /**< \see autocd() */

        bool autocomplete() const;      ///< Get current autocomplete status
                                        /**< if autocomplete is enabled, path components which can
                                             be uniquely completed will be completed
                                             automatically. Disabled by default (but enabled
                                             automatically by the interactive console). */

        Executor & autocomplete(bool v); ///< Set autocomplete status
                                        /**< \see autocomplete() */


        DirectoryNode & chroot() const; ///< Get root node
                                        /**< The root node defaults to senf::console::root(). If
                                             changed, all path references are relative to this node
                                             and objects outside that tree cannot be accessed. */

        Executor & chroot(DirectoryNode & node); ///< chroot into given directory
                                        /**< After this call, all path's are interpreted relative to
                                             \a node and only nodes in the tree rooted at \a node
                                             are accessible via the executor. This value defaults to
                                             senf::console::root(). */

        Executor & policy(SecurityPolicy policy = SecurityPolicy()); ///< Set security policy
                                        /**< The security policy is called before traversing a node
                                             to validate that access. */

    protected:

    private:
        typedef std::vector<DirectoryNode::weak_ptr> Path;

        void exec(std::ostream & output, ParseCommandInfo const & command);

        void cd(ParseCommandInfo::TokensRange dir);
        void ls(std::ostream & output, ParseCommandInfo::TokensRange dir);
        void ll(std::ostream & output, ParseCommandInfo::TokensRange dir);
        void lr(std::ostream & output, ParseCommandInfo::TokensRange dir);
        void echo(std::ostream & output, ParseCommandInfo::TokensRange args);
        void pushd(ParseCommandInfo::TokensRange dir);
        void popd();
        void exit();
        void help(std::ostream & output, ParseCommandInfo::TokensRange path);

        GenericNode & traverseNode(ParseCommandInfo::TokensRange const & path);
        void traverseDirectory(ParseCommandInfo::TokensRange const & path,
                               Path & dir);
        std::string complete(DirectoryNode & dir, std::string const & name);

        struct InvalidPathException {
            std::string path;
            InvalidPathException() : path() {}
            InvalidPathException(std::string path_) : path(path_) {}

        };
        struct InvalidDirectoryException {
            std::string path;
            InvalidDirectoryException() : path() {}
            InvalidDirectoryException(std::string path_) : path(path_) {}
        };
        struct InvalidCommandException {};

        DirectoryNode::ptr root_;
        SecurityPolicy policy_;
        mutable Path cwd_;
        Path oldCwd_;

        typedef std::vector<Path> DirStack;
        DirStack dirstack_;

        bool autocd_;
        bool autocomplete_;
    };

    void senf_console_format_value(DirectoryNode::ptr value, std::ostream & os);

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Executor.cci"
//#include "Executor.ct"
//#include "Executor.cti"
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
