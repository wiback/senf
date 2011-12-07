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
    \brief StreamRegistry public header */

#ifndef HH_SENF_Utils_Logger_StreamRegistry_
#define HH_SENF_Utils_Logger_StreamRegistry_ 1

// Custom includes
#include <map>
#include <ext/functional>
#include <boost/iterator/transform_iterator.hpp>
#include <senf/Utils/singleton.hh>

//#include "StreamRegistry.mpp"
#include "StreamRegistry.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    namespace detail { struct StreamBase; }

    /** \brief Stream registry

        The stream registry keeps track of all streams defined. stream classes are defined as
        singletons and will automatically register with this registry.

        The stream registry exposes a forward sequence interface which is a sequence of the names of
        all registered streams.
     */
    class StreamRegistry
        : public senf::singleton<StreamRegistry>
    {
        typedef std::map<std::string, detail::StreamBase const *> Registry;

    public:
        typedef boost::transform_iterator< ::__gnu_cxx::select1st<Registry::value_type>,
                                           Registry::const_iterator > iterator;

#       ifdef DOXYGEN
        // Hmm ... doxygen does not understand 'using declarations' ...
        /// Access stream registry singleton instance
        static AreaRegistry & instance();
#       endif

        using senf::singleton<StreamRegistry>::instance;

        iterator begin();
        iterator end();

        detail::StreamBase const * lookup(std::string const & name);

    private:
        StreamRegistry();

        void registerStream(detail::StreamBase const & stream);

        Registry registry_;

        friend class senf::singleton<StreamRegistry>;
        friend struct detail::StreamBase;
        friend class Target;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "StreamRegistry.cci"
//#include "StreamRegistry.ct"
//#include "StreamRegistry.cti"
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
