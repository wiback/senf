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
    \brief ConfigFile public header */

#ifndef HH_SENF_Scheduler_Console_ConfigFile_
#define HH_SENF_Scheduler_Console_ConfigFile_ 1

// Custom includes
#include "Config.hh"

//#include "ConfigFile.mpp"
#include "ConfigFile.ih"
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

        If your application uses multiple configuration sources, use a ConfigBundle and FileConfig

        \ingroup console_access
      */
    class ConfigFile 
        : public detail::BundleMixin
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

        void ignoreMissing();           ///< Call to ignore missing files

    private:
        detail::ConfigFileSource & self_;
    };

    /** \brief Read configuration file

        The configuration file \a filename will be loaded, interpreting all node's relative to \a
        root as root node.

        This function uses a local ConfigFile object to perform the parsing.

        \related ConfigFile
     */
    void parseFile(std::string const & filename, DirectoryNode & root = root());

    /** \brief ConfigBundle source reading a configuration file

        This constructor is used to create a config source parsing the given configuration file to
        add to a ConfigBundle.

        \related ConfigFile
     */
    detail::ConfigFileSource::ptr FileConfig(std::string const & filename);

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "ConfigFile.cci"
//#include "ConfigFile.ct"
//#include "ConfigFile.cti"
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
