// $Id$
//
// Copyright (C) 2007
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
    \brief AreaRegistry public header */

#ifndef HH_SENF_Utils_Logger_AreaRegistry_
#define HH_SENF_Utils_Logger_AreaRegistry_ 1

// Custom includes
#include <map>
#include <functional>
#include <boost/iterator/transform_iterator.hpp>
#include <senf/Utils/singleton.hh>

//#include "AreaRegistry.mpp"
#include "AreaRegistry.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    namespace detail { struct AreaBase; }

    /** \brief Area registry

        The area registry keeps track of all areas defined.

        The area registry exposes a forward sequence interface which allows to query the list of all
        registered areas.

        \implementation Area classes are defined as singletons and will automatically register with
        this registry.
     */
    class AreaRegistry
        : public senf::singleton<AreaRegistry>
    {
        typedef std::map<std::string, detail::AreaBase const *> Registry;

        struct SelectName
        {
            typedef std::string result_type;
            std::string const & operator()(Registry::value_type const & v) const;
        };

    public:
        typedef boost::transform_iterator<SelectName, Registry::const_iterator> iterator;
                                        ///< Iterator type

#       ifdef DOXYGEN
        // Hmm ... doxygen does not understand using declarations ...
        /// Access area registry singleton instance
        static AreaRegistry & instance();
#       endif

        using senf::singleton<AreaRegistry>::instance;

        iterator begin();               ///< Beginning of area name sequence
        iterator end();                 ///< End of area name sequence

        detail::AreaBase const * lookup(std::string const & name);

    private:
        AreaRegistry();

        void registerArea(detail::AreaBase const & area);

        Registry registry_;

        friend class senf::singleton<AreaRegistry>;
        friend class detail::AreaBase;
        friend class Target;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "AreaRegistry.cci"
//#include "AreaRegistry.ct"
//#include "AreaRegistry.cti"
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
