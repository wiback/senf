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
    \brief ConsoleTarget public header */

#ifndef HH_SENF_Utils_Logger_ConsoleTarget_
#define HH_SENF_Utils_Logger_ConsoleTarget_ 1

// Custom includes
#include "IOStreamTarget.hh"

//#include "ConsoleTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Write %log messages to std::cout

        IOStreamTarget writing to std::cout. This is a singleton target which always exists. Access
        it via senf::log::ConsoleTarget::instance()
        \code
        senf::log::ConsoleTarget & console (senf::log::ConsoleTarget::instance());

        console.route<senf::log::Debug>();
        \endcode

        \ingroup targets
     */
    class ConsoleTarget : public IOStreamTarget
    {
    public:
        static ConsoleTarget & instance();

        static void logToStderr();

    private:
        ConsoleTarget();
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ConsoleTarget.cci"
//#include "ConsoleTarget.ct"
//#include "ConsoleTarget.cti"
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
