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
        friend struct detail::AreaBase;
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
