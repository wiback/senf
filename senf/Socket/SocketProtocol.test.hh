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
