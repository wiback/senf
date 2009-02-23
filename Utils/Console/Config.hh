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

#ifndef HH_SENF_Scheduler_Console_Config_
#define HH_SENF_Scheduler_Console_Config_ 1

// Custom includes
#include <boost/utility.hpp>
#include <list>
#include "Parse.hh"
#include "Executor.hh"

//#include "Config.mpp"
#include "Config.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    /** \brief Combine multiple configuration sources

        A ConfigBundle combines several sources and parses them together, in the order they were
        added. Parse restrictions are applied uniformly to all sources.
        \code
        // Add three configuration sources: A system configuration file, a user configuration file
        // and command line options
        senf::console::ConfigBundle conf;
        conf.add( senf::console::FileConfig("/etc/some.conf") );
        conf.add( senf::console::FileConfig("local.conf") );
        conf.add( senf::console::OptionsConfig(senf::Daemon::instance().argc(),
                                               senf::Daemon::instance().argv()) );

        conf.parse();
        \endcode

        This bundle may also be passed to other code which may use restricted parsing to parse
        partial information from all configuration sources.
        
        \ingroup console_access
      */
    class ConfigBundle
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ConfigBundle();
        ConfigBundle(DirectoryNode & root); ///< Set custom root node

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        template <class Source>
        Source & add(boost::intrusive_ptr<Source> source);
                                        ///< Add configuration source
       
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
        void parseInternal();

        typedef std::list<detail::ConfigSource::ptr> Sources;

        Sources sources_;
        detail::RestrictedExecutor executor_;
    };

namespace detail {
    // hrmpf ... Can't place this into Config.ih ...

    /** \brief Internal: Provide ConfigBundle facade for a single-source config.

        The BundleMixin is used to define supplementary configuration objects for one specific
        configuration source. A BundleMixin is \e not a ConfigBundle since it has no public \c add()
        member.
     */
    class BundleMixin
    {
    public:
        BundleMixin();
        BundleMixin(DirectoryNode & root);

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
        template <class Source>
        Source & add(boost::intrusive_ptr<Source> source);
        
    private:
        ConfigBundle bundle_;
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Config.cci"
//#include "Config.ct"
#include "Config.cti"
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
