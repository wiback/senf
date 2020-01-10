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
    \brief net.test public header */

#ifndef HH_SENF_Socket_Protocols_INet_net_test_
#define HH_SENF_Socket_Protocols_INet_net_test_ 1

// Custom includes
#include <string>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace test {

    void error(char const * fn, char const * proc="");

    void fail(char const * proc, char const * fn);

    void start(void (*fn)());

    void wait();

    void stop();

    unsigned port(unsigned i);

    std::string localhost4str(unsigned i);

    std::string localhost6str(unsigned i);

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "net.test.cci"
//#include "net.test.ct"
//#include "net.test.cti"
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
