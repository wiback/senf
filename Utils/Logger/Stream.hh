// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Stream public header */

#ifndef HH_Stream_
#define HH_Stream_ 1

// Custom includes
#include <map>
#include <functional>
#include <boost/iterator/transform_iterator.hpp>
#include "Levels.hh"
#include "../singleton.hh"

//#include "Stream.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief Define log stream

    Defines a new log stream named \a stream. The stream is defined as a symbol in the current
    scope.

    \a defaultLevel defines the default log level for messages posted to this stream. \a
    runtimeLimit defines the default log limit. Messages with a level below this will not be
    output.  \a compileLimit defines the default log level limit at compile time: Messages
    posted with a level below \a compileLimit will be discarded at compile time.

    \hideinitializer
 */
#define SENF_LOG_DEF_STREAM(stream, defaultLevel_, runtimeLimit_, compileLimit_)                  \
    struct stream                                                                                 \
        : public senf::log::detail::StreamBase, public senf::singleton<stream>                    \
    {                                                                                             \
        typedef defaultLevel_ defaultLevel;                                                       \
        typedef runtimeLimit_ runtimeLimit;                                                       \
        typedef compileLimit_ compileLimit;                                                       \
        static std::string name() { return instance().v_name(); }                                 \
    private:                                                                                      \
        stream() { init(); }                                                                      \
        friend class senf::singleton<stream>;                                                     \
    }

namespace senf {
namespace log {

    namespace detail { struct StreamBase; }

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

        using senf::singleton<StreamRegistry>::instance;

        iterator begin();
        iterator end();

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
#include "Stream.cci"
//#include "Stream.ct"
//#include "Stream.cti"
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
