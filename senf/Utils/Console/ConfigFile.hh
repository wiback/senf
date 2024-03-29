//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ConfigFile public header */

#ifndef HH_SENF_Utils_Console_ConfigFile_
#define HH_SENF_Utils_Console_ConfigFile_ 1

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

        explicit ConfigFile(std::string const & filename, DirectoryNode & root = senf::console::root());
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
    void parseFile(std::string const & filename, DirectoryNode & root = senf::console::root());

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
