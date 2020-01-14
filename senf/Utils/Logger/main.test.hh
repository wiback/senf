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
    \brief main.test public header */

#ifndef HH_SENF_Utils_Logger_main_test_
#define HH_SENF_Utils_Logger_main_test_ 1

// Custom includes

#ifdef SENF_LOG_CONF
#undef SENF_LOG_CONF
#endif

#define SENF_LOG_CONF (( (senf)(log)(Debug), (_), NOTICE )) \
                      (( (senf)(log)(test)(myStream), (GlobalTestArea), IMPORTANT )) \
                      (( (senf)(log)(test)(myStream), (senf)(log)(test)(Foo), VERBOSE ))

#include "Logger.hh"

//#include "main.test.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_LOG_DEFINE_AREA(GlobalTestArea);

namespace senf {
namespace log {
namespace test {

    struct Foo
    {
        SENF_LOG_CLASS_AREA();

        static void log() {
            SENF_LOG(("Foo::log"));
        }
    };

    SENF_LOG_DEFINE_ALIAS( LogCritical, (senf::log::Debug) (senf::log::CRITICAL) );
    SENF_LOG_DEFINE_STREAM( myStream, senf::log::MESSAGE, senf::log::MESSAGE, senf::log::MESSAGE );
    SENF_LOG_DEFINE_AREA( myArea );

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "main.test.cci"
//#include "main.test.ct"
//#include "main.test.cti"
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
