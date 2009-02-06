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
    \brief StreamRegistry public header */

#ifndef HH_SENF_Utils_Logger_StreamRegistry_
#define HH_SENF_Utils_Logger_StreamRegistry_ 1

// Custom includes
#include <map>
#include <functional>
#include <boost/iterator/transform_iterator.hpp>
#include "Levels.hh"
#include "../singleton.hh"

//#include "StreamRegistry.mpp"
#include "StreamRegistry.ih"
///////////////////////////////hh.p////////////////////////////////////////

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

        struct SelectName 
        {
            typedef std::string result_type;
            std::string const & operator()(Registry::value_type const & v) const;
        };

    public:
        typedef boost::transform_iterator<SelectName, Registry::const_iterator> iterator;

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
        friend class detail::StreamBase;
        friend class Target;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////#
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
