// $Id$
//
// Copyright (C) 2009
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
