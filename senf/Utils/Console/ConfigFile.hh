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
    \brief ConfigFile public header */

#ifndef HH_SENF_Scheduler_Console_ConfigFile_
#define HH_SENF_Scheduler_Console_ConfigFile_ 1

// Custom includes
#include "Config.hh"

//#include "ConfigFile.mpp"
#include "ConfigFile.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit ConfigFile(std::string const & filename, DirectoryNode & root = root());
                                        ///< Create ConfigFile object for \a filename
                                        /**< The \a filename configuration file will be parsed using
                                             parse() calls. All configuration statements will be
                                             interpreted relative to \a root as root node. */

        //\}
        //-////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
