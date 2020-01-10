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
    \brief StringTarget public header */

#ifndef HH_SENF_Utils_Logger_StringTarget_
#define HH_SENF_Utils_Logger_StringTarget_ 1

// Custom includes
#include <sstream>
#include "IOStreamTarget.hh"

//#include "StringTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Store %log messages in a string buffer

        This target is mostly useful for debug purposes.

        \ingroup targets
      */
    class StringTarget
        : private boost::base_from_member<std::stringstream>,
          public IOStreamTarget
    {
        typedef boost::base_from_member<std::stringstream> stream_base;
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        StringTarget();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        std::string str() const;        ///< Get log messages accumulated so far
        void clear();                   ///< Clear buffer

    protected:

    private:

    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "StringTarget.cci"
//#include "StringTarget.ct"
#include "StringTarget.cti"
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
