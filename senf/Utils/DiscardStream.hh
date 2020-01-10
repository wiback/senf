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
    \brief DiscardStream public header */

#ifndef HH_SENF_Utils_DiscardStream_
#define HH_SENF_Utils_DiscardStream_ 1

// Custom includes
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

//#include "DiscardStream.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class DiscardSink
        : public boost::iostreams::sink
    {
    public:
        std::streamsize write(char const * s, std::streamsize n);
    };

    class DiscardStream
        : public boost::iostreams::stream<DiscardSink>
    {
    public:
        DiscardStream();
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "DiscardStream.cci"
//#include "DiscardStream.ct"
//#include "DiscardStream.cti"
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
