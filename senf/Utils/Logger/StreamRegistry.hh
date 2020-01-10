//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
