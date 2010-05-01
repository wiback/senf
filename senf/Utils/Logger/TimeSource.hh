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
    \brief TimeSource public header */

#ifndef HH_SENF_Utils_Logger_TimeSource_
#define HH_SENF_Utils_Logger_TimeSource_ 1

// Custom includes
#include <boost/cstdint.hpp>
#include <memory>
#include <senf/config.hh>

//#include "TimeSource.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    typedef config::time_type time_type;

    /** \brief Log message time source abstract base class

        Instances derived from TimeSource provide the Logging library with the current date/time
        value. The \c operator() member must be implemented to return the current universal time
        (UTC).

        A new TimeSource may be installed using \ref senf::log::timeSource().

        \ingroup config
     */
    struct TimeSource
    {
        virtual ~TimeSource();
        virtual time_type operator()() const = 0;

        static time_type now();
    };

    /** \brief Default %log message time source

        This time source is installed by default and uses gettimeofday() (via the Boost.DateTime
        library) to get the current universal time.

        \ingroup config
     */
    struct SystemTimeSource : public TimeSource
    {
        virtual time_type operator()() const;
    };

    /** \brief Change %log message time source

        Set the %log message time source to \a source. The logging library will take ownership of \e
        source and will take care to free it, if necessary.

        Since the time source class will in almost all cases be default constructible, see the
        template overload for a simpler interface.

        \ingroup config
     */
    void timeSource(std::auto_ptr<TimeSource> source);

    /** \brief Change %log message time source

        Set the %log message time source to (an instance of) \a Source.  \a Source must be default
        constructible, otherwise use the non-template senf::log::timeSource() overload.

        \ingroup config
     */
    template <class Source> void timeSource();

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "TimeSource.cci"
//#include "TimeSource.ct"
#include "TimeSource.cti"
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
