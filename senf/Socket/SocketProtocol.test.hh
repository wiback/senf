// $Id$
//
// Copyright (C) 2006
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


#ifndef HH_SENF_Socket_SocketProtocol_test_
#define HH_SENF_Socket_SocketProtocol_test_ 1

// Custom includes
#include "SocketProtocol.hh"
#include "SocketPolicy.test.hh"
#include "ProtocolClientSocketHandle.hh"

//#include "SocketProtocol.test.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace test {

    class SomeSocketProtocol
        : public ConcreteSocketProtocol<SomeSocketPolicy,SomeSocketProtocol>
    {
    public:
        ~SomeSocketProtocol() {}

        void init_client() const { fd(0); }
        void init_server() const { fd(0); }

        unsigned available() const
            { return Policy::ReadPolicy::TEST_SIZE; }
        bool eof() const
            { return false; }

        virtual void close() {
            closeCount(1);
        }

        static unsigned closeCount(unsigned inc=0) {
            static unsigned counter (0);
            counter += inc;
            return counter;
        }
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "SocketProtocol.test.cci"
//#include "SocketProtocol.test.ct"
//#include "SocketProtocol.test.cti"
//#include "SocketProtocol.test.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
