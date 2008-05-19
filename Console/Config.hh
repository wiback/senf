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
    \brief Config public header */

#ifndef HH_Console_Config_
#define HH_Console_Config_ 1

// Custom includes
#include <boost/utility.hpp>
#include "Parse.hh"
#include "Executor.hh"

//#include "Config.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Console node tree based config file parser

        A ConfigFile instance allows flexible parsing of a config file against the console node
        tree. If you just want to parse a file completely, the senf::console::readConfig() function
        will do that. ConfigFile however allows to incrementally parse only a subdirectory of the
        complete configuration file.
        \code
        senf::console::ConfigFile cf ("/my/config/file")

        // Parse only statements under the directory of some object. The object 'ob'
        // must have been registered somewhere in the node tree
        cf.parse(ob.dir);
        
        // Parse rest of the config file
        cf.parse();
        \endcode
      */
    class ConfigFile
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit ConfigFile(std::string const & filename, DirectoryNode & root = root());
                                        ///< Create ConfigFile object for \a filename
                                        /**< The \a filename configuration file will be parsed using
                                             parse() calls. All configuration statements will be
                                             interpreted relative to \a root as root node. */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void parse();                   ///< Parse config file
                                        /**< All nodes already parsed are skipped */
        void parse(DirectoryNode & restrict); ///< Parse config file under \a restrict
                                        /**< Only nodes which are children of \a restrict are
                                             parsed.  */

        bool complete() const;          ///< \c true, if all nodes have been parsed
        bool parsed(GenericNode & node) const; ///< \c true. if \a node has been parsed

        void reset();                   ///< Reset node parse info state
                                        /**< After a call to reset(), all information about already
                                             parsed nodes is cleared. Calling parse() will parse the
                                             complete config file again. */

    protected:

    private:
        void policyCallback(DirectoryNode::ptr restrict, DirectoryNode & dir, 
                            std::string const & item);

        void insertParsedNode(DirectoryNode::ptr node);

        typedef std::vector<DirectoryNode::weak_ptr> ParsedNodes;

        std::string filename_;
        CommandParser parser_;
        Executor executor_;

        ParsedNodes parsedNodes_;
    };

    /** \brief Read configuration file

        The configuration file \a filename will be loaded, interpreting all node's relative to \a
        root as root node.

        This function uses a local ConfigFile object to perform the parsing.

        \related ConfigFile
     */
    void readConfig(std::string const & filename, DirectoryNode & root = root());

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Config.cci"
//#include "Config.ct"
//#include "Config.cti"
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
