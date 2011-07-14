// $Id$
//
// Copyright (C) 2008
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
    \brief TimeSource public header */

#ifndef HH_SENF_Utils_Logger_TimeSource_
#define HH_SENF_Utils_Logger_TimeSource_ 1

// Custom includes
#include <boost/cstdint.hpp>
#include <memory>
#include <senf/config.hh>

//#include "TimeSource.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
