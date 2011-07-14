// $Id$
//
// Copyright (C) 2007
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
    \brief AreaRegistry public header */

#ifndef HH_SENF_Utils_Logger_AreaRegistry_
#define HH_SENF_Utils_Logger_AreaRegistry_ 1

// Custom includes
#include <map>
#include <boost/iterator/transform_iterator.hpp>
#include <senf/Utils/singleton.hh>

//#include "AreaRegistry.mpp"
#include "AreaRegistry.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
