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
    \brief TimeSource public header */

#ifndef HH_SENF_Utils_Logger_TimeSource_
#define HH_SENF_Utils_Logger_TimeSource_ 1

// Custom includes
#include <memory>
#include <senf/Utils/Cpp11Support/smart_ptr.hh>
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
    void timeSource(std::unique_ptr<TimeSource> source);

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
